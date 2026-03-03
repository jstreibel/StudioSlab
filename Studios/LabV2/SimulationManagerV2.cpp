#include "SimulationManagerV2.h"

#include "imgui.h"

#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Core/SlabCore.h"
#include "Core/Tools/Log.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"
#include "Studios/Common/Simulations/V2/KGR2toRControlTopicsV2.h"
#include "Studios/Common/Simulations/V2/LiveParameterControlV2.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <numbers>
#include <type_traits>
#include <utility>

namespace {

    auto AddTooltipForLastItem(const char *tooltip) -> void {
        if (tooltip == nullptr || !ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) return;
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 36.0f);
        ImGui::TextUnformatted(tooltip);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

    template<typename TInt>
    auto DragUIntLike(const char *label,
                      TInt &value,
                      const TInt minVal,
                      const TInt maxVal,
                      const float speed = 1.0f,
                      const char *tooltip = nullptr)
        -> bool {
        static_assert(std::is_integral_v<TInt>);
        long long tmp = static_cast<long long>(value);
        const bool changed = ImGui::DragScalar(label, ImGuiDataType_S64, &tmp, speed, nullptr, nullptr);
        AddTooltipForLastItem(tooltip);
        if (changed) {
            tmp = std::max<long long>(static_cast<long long>(minVal), tmp);
            tmp = std::min<long long>(static_cast<long long>(maxVal), tmp);
            value = static_cast<TInt>(tmp);
        }
        return changed;
    }

    auto DragDevFloat(const char *label,
                      Slab::DevFloat &value,
                      const double speed,
                      const double minVal,
                      const double maxVal,
                      const char *format = "%.6g",
                      const char *tooltip = nullptr) -> bool {
        const bool changed = ImGui::DragScalar(label, ImGuiDataType_Double, &value, speed, &minVal, &maxVal, format);
        AddTooltipForLastItem(tooltip);
        return changed;
    }

    auto CheckboxWithTooltip(const char *label, bool *bValue, const char *tooltip) -> bool {
        const bool changed = ImGui::Checkbox(label, bValue);
        AddTooltipForLastItem(tooltip);
        return changed;
    }

    auto ButtonWithTooltip(const char *label, const char *tooltip) -> bool {
        const bool pressed = ImGui::Button(label);
        AddTooltipForLastItem(tooltip);
        return pressed;
    }

    auto PublishLauncherScalarControl(const Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2> &hub,
                                      const Slab::Str &topicPrefix,
                                      const Slab::Str &key,
                                      const Slab::DevFloat value) -> void {
        const auto topicName = Slab::Studios::Common::Simulations::V2::BuildLiveScalarParameterTopicV2(topicPrefix, key);
        Slab::Studios::Common::Simulations::V2::PublishLiveScalarParameterValueV2(hub, topicName, value);
    }

    template<typename TConfig>
    auto ApplyCommonExecutionSettings(TConfig &cfg,
                                      const Slab::UIntBig interval,
                                      const Slab::UIntBig monitorInterval,
                                      const Slab::UIntBig batch) -> void {
        cfg.Interval = interval;
        cfg.MonitorInterval = monitorInterval;
        cfg.Batch = batch;
    }

} // namespace

FSimulationManagerV2::FSimulationManagerV2(
    Slab::TPointer<Slab::Graphics::FImGuiContext> imGuiContext,
    Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2> liveDataHub,
    Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2> liveControlHub,
    FAddWindowFn addWindowFn,
    FRequestLauncherVisibilityFn requestLauncherVisibilityFn)
: ImGuiContext(std::move(imGuiContext))
, LiveDataHub(std::move(liveDataHub))
, LiveControlHub(std::move(liveControlHub))
, AddWindow(std::move(addWindowFn))
, RequestLauncherVisibility(std::move(requestLauncherVisibilityFn)) {
    ApplyCommonExecutionSettings(SPICfg, CommonInterval, CommonMonitorInterval, CommonBatch);

    ApplyCommonExecutionSettings(RtoRCfg, CommonInterval, CommonMonitorInterval, CommonBatch);
    RtoRCfg.bHistorySummary = true;

    ApplyCommonExecutionSettings(R2toRCfg, CommonInterval, CommonMonitorInterval, CommonBatch);
    R2toRCfg.bEnableLiveControlForcing = true;
    R2toRCfg.ControlSampleInterval = 1;
    R2toRCfg.ControlTopicPrefix = Slab::Studios::Common::Simulations::V2::KG2DControlTopicPrefixDefaultV2;
    R2toRCfg.bEnableMonitorControlPublisher = false;
    R2toRCfg.ForcingWidth = 0.35;
    R2toRCfg.ForcingAmplitude = 0.0;
    R2toRCfg.bForcingEnabled = false;

    MolecularDynamicsCfg.N = 256;
    MolecularDynamicsCfg.L = 50.0;
    MolecularDynamicsCfg.TotalTime = 20.0;
    MolecularDynamicsCfg.Steps = 400;
    MolecularDynamicsCfg.Temperature = 0.0;
    MolecularDynamicsCfg.Dissipation = 0.0;
    ApplyCommonExecutionSettings(MolecularDynamicsCfg, CommonInterval, CommonMonitorInterval, CommonBatch);

    XYCfg.L = 64;
    XYCfg.Steps = 2000;
    XYCfg.Temperature = 0.7;
    XYCfg.ExternalField = 0.0;
    XYCfg.DeltaTheta = 2.0 * std::numbers::pi_v<Slab::DevFloat>;
    ApplyCommonExecutionSettings(XYCfg, CommonInterval, CommonMonitorInterval, CommonBatch);
    XYCfg.bEnableLiveParameterBinding = true;
    XYCfg.ControlSampleInterval = 1;
    XYCfg.ControlTopicPrefix = Slab::Studios::Common::Simulations::V2::XYControlTopicPrefixDefaultV2;

    IsingCfg.L = 64;
    IsingCfg.Steps = 2000;
    IsingCfg.Temperature = 2.269185314;
    IsingCfg.ExternalField = 0.0;
    ApplyCommonExecutionSettings(IsingCfg, CommonInterval, CommonMonitorInterval, CommonBatch);
    IsingCfg.bEnableLiveParameterBinding = true;
    IsingCfg.ControlSampleInterval = 1;
    IsingCfg.ControlTopicPrefix = Slab::Studios::Common::Simulations::V2::IsingControlTopicPrefixDefaultV2;

    ApplyCommonExecutionSettings(MetropolisCfg, CommonInterval, CommonMonitorInterval, CommonBatch);
}

auto FSimulationManagerV2::AddMenus(const Slab::Graphics::FPlatformWindow &platformWindow) -> void {
    if (ImGuiContext == nullptr) return;

    {
        const auto itemLocation = Slab::Graphics::MainMenuLocation{"Lab"};
        const auto item = Slab::Graphics::MainMenuItem{
            itemLocation,
            {
                {"Launcher", "Show simulation launcher"}
            },
            [this](const Slab::Str &itemString) {
                if (itemString == "Launcher") {
                    RequestLauncherVisible();
                    return;
                }
            }
        };
        ImGuiContext->AddMainMenuItem(item);
    }

    {
        const auto itemLocation = Slab::Graphics::MainMenuLocation{"Simulations"};
        const auto item = Slab::Graphics::MainMenuItem{
            itemLocation,
            {
                {"SPI"},
                {"KGRtoR Plane Waves"},
                {"KGR2toR"},
                {Slab::Graphics::MainMenuSeparator},
                {"Molecular Dynamics"},
                {Slab::Graphics::MainMenuSeparator},
                {"XY Metropolis"},
                {Slab::Graphics::MainMenuSeparator},
                {"Ising Metropolis"},
                {Slab::Graphics::MainMenuSeparator},
                {"Metropolis RtoR"},
                {Slab::Graphics::MainMenuSeparator},
                {"Open Launcher"}
            },
            [this, &platformWindow](const Slab::Str &itemString) {
                (void) platformWindow;
                try {
                    if (HandleLaunchMenuItem(itemString)) return;
                } catch (const std::exception &e) {
                    CaptureLaunchError(e);
                }
            }
        };
        ImGuiContext->AddMainMenuItem(item);
    }
}

auto FSimulationManagerV2::HandleLaunchMenuItem(const Slab::Str &itemString) -> bool {
    struct FLaunchMenuEntry {
        const char *Label;
        void (FSimulationManagerV2::*LaunchFn)(bool);
    };

    static constexpr std::array<FLaunchMenuEntry, 7> entries = {{
        {"SPI", &FSimulationManagerV2::LaunchSPI},
        {"KGRtoR Plane Waves", &FSimulationManagerV2::LaunchRtoR},
        {"KGR2toR", &FSimulationManagerV2::LaunchR2toR},
        {"Molecular Dynamics", &FSimulationManagerV2::LaunchMolecularDynamics},
        {"XY Metropolis", &FSimulationManagerV2::LaunchXY},
        {"Ising Metropolis", &FSimulationManagerV2::LaunchIsing},
        {"Metropolis RtoR", &FSimulationManagerV2::LaunchMetropolis}
    }};

    for (const auto &entry : entries) {
        if (itemString != entry.Label) continue;
        (this->*entry.LaunchFn)(bStartWithMonitor);
        return true;
    }

    if (itemString == "Open Launcher") {
        RequestLauncherVisible();
        return true;
    }

    return false;
}

auto FSimulationManagerV2::CaptureLaunchError(const std::exception &e) -> void {
    LastError = e.what();
    Slab::Core::Log::Error() << "Lab launch failed: " << e.what() << Slab::Core::Log::Flush;
}

auto FSimulationManagerV2::RequestLauncherVisible() const -> void {
    if (RequestLauncherVisibility != nullptr) {
        RequestLauncherVisibility();
    }
}

auto FSimulationManagerV2::GetOrCreateLiveViewIfNeeded(const bool bNeedLiveView,
                                                        const Slab::Str &prefix,
                                                        Slab::UIntBig &counter)
    -> Slab::TPointer<Slab::Math::LiveData::V2::FSessionLiveViewV2> {
    if (!bNeedLiveView || LiveDataHub == nullptr) return nullptr;
    return LiveDataHub->GetOrCreateSessionLiveView(MakeTopicName(prefix, counter));
}

auto FSimulationManagerV2::AttachMonitorWindowOrThrow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window,
                                                       const char *context) const -> void {
    if (window == nullptr) {
        throw Exception(Slab::Str(context) + " returned null monitor window.");
    }
    if (!AddWindow) throw Exception("Lab cannot attach monitor window.");
    AddWindow(window);
}

auto FSimulationManagerV2::DrawLauncherContents() -> void {
    ImGui::TextDisabled("Examples launcher");
    if (!LastError.empty()) {
        ImGui::SeparatorText("Last Error");
        ImGui::TextWrapped("%s", LastError.c_str());
        if (ButtonWithTooltip("Clear Error", "Clear the latest launch error message.")) LastError.clear();
    }

    ImGui::SeparatorText("Common");
    DragUIntLike("Interval", CommonInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Console/log sampling interval (steps).");
    DragUIntLike("Monitor interval", CommonMonitorInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Snapshot publishing interval for monitor/live data.");
    DragUIntLike("Batch", CommonBatch, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Max internal steps per worker loop iteration.");
    CheckboxWithTooltip("Publish live data when headless", &bPublishLiveDataWhenHeadless, "Expose telemetry/live data even without GL monitor.");
    CheckboxWithTooltip("Start with monitor", &bStartWithMonitor, "Launch each simulation with the GL monitor window enabled.");

    DrawSPISection();
    DrawRtoRSection();
    DrawR2toRSection();
    DrawMolecularDynamicsSection();
    DrawXYSection();
    DrawIsingSection();
    DrawMetropolisSection();
}

auto FSimulationManagerV2::TickLiveControlPublishers() -> void {
    if (XYCfg.bEnableLiveParameterBinding && bXYPublishLiveParameters) {
        PublishLauncherScalarControl(LiveControlHub, XYCfg.ControlTopicPrefix, "temperature", XYCfg.Temperature);
        PublishLauncherScalarControl(LiveControlHub, XYCfg.ControlTopicPrefix, "external_field", XYCfg.ExternalField);
    }
    if (IsingCfg.bEnableLiveParameterBinding && bIsingPublishLiveParameters) {
        PublishLauncherScalarControl(LiveControlHub, IsingCfg.ControlTopicPrefix, "temperature", IsingCfg.Temperature);
        PublishLauncherScalarControl(LiveControlHub, IsingCfg.ControlTopicPrefix, "external_field", IsingCfg.ExternalField);
    }
}

auto FSimulationManagerV2::DrawSPISection() -> void {
    if (!ImGui::CollapsingHeader("SPI")) return;

    DragUIntLike("Steps##spi", SPICfg.Steps, 1u, 100000000u, 1.0f, "Number of stochastic-time integration steps.");
    DragDevFloat("dT##spi", SPICfg.Dt, 0.0005, 1e-6, 100.0, "%.6g", "Stochastic-time step size.");
    DragDevFloat("Time##spi", SPICfg.Time, 0.01, 1e-6, 1e6, "%.6g", "Physical time horizon.");
    DragDevFloat("L##spi", SPICfg.L, 0.01, 1e-6, 1e6, "%.6g", "Spatial domain length.");
    DragUIntLike("N##spi", SPICfg.N, 2u, 200000u, 1.0f, "Number of lattice sites.");
    const char *runTooltip = bStartWithMonitor
        ? "Run and open the passive GL monitor for this simulation."
        : "Run this simulation without opening a GL monitor window.";
    if (ButtonWithTooltip("Run##spi", runTooltip)) {
        try { LaunchSPI(bStartWithMonitor); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::DrawRtoRSection() -> void {
    if (!ImGui::CollapsingHeader("KGRtoR Plane Waves")) return;

    DragUIntLike("Steps##rtor", RtoRCfg.Steps, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Total integration steps.");

    bool bUseDtOverride = RtoRCfg.Dt > 0.0;
    if (CheckboxWithTooltip("Override dT (otherwise legacy dt/h=0.1)##rtor", &bUseDtOverride, "Use explicit dt instead of legacy dt/h rule.")) {
        if (!bUseDtOverride) RtoRCfg.Dt = -1.0;
        else if (RtoRCfg.Dt <= 0.0) RtoRCfg.Dt = 0.01;
    }
    if (bUseDtOverride) {
        DragDevFloat("dT##rtor", RtoRCfg.Dt, 0.0001, 1e-8, 100.0, "%.6g", "Time step for integrator.");
    } else {
        ImGui::TextDisabled("dt auto = 0.1 * (L / N)");
    }

    DragDevFloat("L##rtor", RtoRCfg.L, 0.01, 1e-6, 1e6, "%.6g", "Domain length.");
    DragUIntLike("N##rtor", RtoRCfg.N, 4u, 200000u, 1.0f, "Spatial resolution (grid points).");
    DragDevFloat("x-center##rtor", RtoRCfg.XCenter, 0.01, -1e6, 1e6, "%.6g", "Initial wave packet center.");
    DragDevFloat("Q##rtor", RtoRCfg.Q, 0.01, -1e6, 1e6, "%.6g", "Initial wave number / momentum.");
    DragUIntLike("Harmonic##rtor", RtoRCfg.Harmonic, 1u, 100000u, 1.0f, "Harmonic mode index.");

    ImGui::TextDisabled("Advanced analysis listeners are currently exposed via CLI (Studios rtor).");

    const char *runTooltip = bStartWithMonitor
        ? "Run and open the passive GL monitor for this simulation."
        : "Run this simulation without opening a GL monitor window.";
    if (ButtonWithTooltip("Run##rtor", runTooltip)) {
        try { LaunchRtoR(bStartWithMonitor); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::DrawR2toRSection() -> void {
    if (!ImGui::CollapsingHeader("KGR2toR")) return;

    DragUIntLike("Steps##kg2d", R2toRCfg.Steps, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Total integration steps.");
    DragDevFloat("L##kg2d", R2toRCfg.L, 0.01, 1e-6, 1e6, "%.6g", "Domain size.");
    DragUIntLike("N##kg2d", R2toRCfg.N, 8u, 200000u, 1.0f, "Grid resolution along each axis.");
    DragDevFloat("r_dt##kg2d", R2toRCfg.RDt, 0.001, 1e-8, 10.0, "%.6g", "CFL-like ratio used to compute dt.");
    ImGui::TextDisabled("dt auto = r_dt * (L / N)");

    DragDevFloat("x-center##kg2d", R2toRCfg.XCenter, 0.01, -1e6, 1e6, "%.6g", "Initial pulse center (x).");
    DragDevFloat("y-center##kg2d", R2toRCfg.YCenter, 0.01, -1e6, 1e6, "%.6g", "Initial pulse center (y).");
    DragDevFloat("Pulse width##kg2d", R2toRCfg.PulseWidth, 0.001, 1e-8, 1e6, "%.6g", "Initial pulse width.");
    DragDevFloat("phi amplitude##kg2d", R2toRCfg.PhiAmplitude, 0.01, -1e6, 1e6, "%.6g", "Initial phi pulse amplitude.");
    DragDevFloat("dphi/dt amplitude##kg2d", R2toRCfg.DPhiDtAmplitude, 0.01, -1e6, 1e6, "%.6g", "Initial time-derivative pulse amplitude.");
    CheckboxWithTooltip("Enable LiveControl forcing binding##kg2d-bind", &R2toRCfg.bEnableLiveControlForcing, "Read forcing controls from LiveControl topics during run.");
    if (CheckboxWithTooltip("Enable monitor control source UI (GL)##kg2d-monitor-ctrl", &R2toRCfg.bEnableMonitorControlPublisher, "Expose forcing controls in GL monitor and publish to LiveControl topics.")) {
        if (R2toRCfg.bEnableMonitorControlPublisher) {
            R2toRCfg.bEnableLiveControlForcing = true;
        }
    }
    if (R2toRCfg.bEnableLiveControlForcing) {
        DragUIntLike("Control sample interval##kg2d", R2toRCfg.ControlSampleInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "How often forcing topics are polled (steps).");
        DragDevFloat("Forcing x##kg2d", R2toRCfg.ForcingXCenter, 0.01, -1e6, 1e6, "%.6g", "Default forcing center (x).");
        DragDevFloat("Forcing y##kg2d", R2toRCfg.ForcingYCenter, 0.01, -1e6, 1e6, "%.6g", "Default forcing center (y).");
        DragDevFloat("Forcing width##kg2d", R2toRCfg.ForcingWidth, 0.001, 1e-8, 1e6, "%.6g", "Default forcing Gaussian width.");
        DragDevFloat("Forcing amplitude##kg2d", R2toRCfg.ForcingAmplitude, 0.01, -1e6, 1e6, "%.6g", "Default forcing amplitude.");
        CheckboxWithTooltip("Forcing enabled##kg2d", &R2toRCfg.bForcingEnabled, "Enable forcing by default.");
        ImGui::TextDisabled("Control topic prefix: %s", R2toRCfg.ControlTopicPrefix.c_str());
    }
    if (R2toRCfg.bEnableMonitorControlPublisher) {
        ImGui::TextDisabled("GL monitor will expose a control source panel for topic-driven forcing.");
    }

    const char *runTooltip = bStartWithMonitor
        ? "Run and open the passive GL monitor for this simulation."
        : "Run this simulation without opening a GL monitor window.";
    if (ButtonWithTooltip("Run##kg2d", runTooltip)) {
        try { LaunchR2toR(bStartWithMonitor); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::DrawMolecularDynamicsSection() -> void {
    if (!ImGui::CollapsingHeader("Molecular Dynamics")) return;

    DragUIntLike("Steps##md", MolecularDynamicsCfg.Steps, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Total integration steps.");
    DragDevFloat("Total time##md", MolecularDynamicsCfg.TotalTime, 0.01, 1e-6, 1e8, "%.6g", "Physical time horizon.");
    DragDevFloat("L##md", MolecularDynamicsCfg.L, 0.01, 1e-6, 1e8, "%.6g", "Box side length.");
    DragUIntLike("N##md", MolecularDynamicsCfg.N, 128u, 200000u, 1.0f, "Number of particles.");
    DragDevFloat("Temperature##md", MolecularDynamicsCfg.Temperature, 0.01, 0.0, 1e6, "%.6g", "Initial/coupled temperature.");
    DragDevFloat("Dissipation##md", MolecularDynamicsCfg.Dissipation, 0.001, 0.0, 1e6, "%.6g", "Dissipation coefficient.");
    const bool bSoftDisk = MolecularDynamicsCfg.InteractionModel ==
        Slab::Studios::Common::Simulations::V2::EMDInteractionModelV2::SoftDisk;
    if (ImGui::RadioButton("SoftDisk", bSoftDisk)) {
        MolecularDynamicsCfg.InteractionModel = Slab::Studios::Common::Simulations::V2::EMDInteractionModelV2::SoftDisk;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Lennard-Jones", !bSoftDisk)) {
        MolecularDynamicsCfg.InteractionModel = Slab::Studios::Common::Simulations::V2::EMDInteractionModelV2::LennardJones;
    }

    ImGui::TextDisabled("Passive GL monitor shows particle cloud + telemetry.");

    const char *runTooltip = bStartWithMonitor
        ? "Run and open the passive GL monitor for this simulation."
        : "Run this simulation without opening a GL monitor window.";
    if (ButtonWithTooltip("Run##md", runTooltip)) {
        try { LaunchMolecularDynamics(bStartWithMonitor); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::DrawMetropolisSection() -> void {
    if (!ImGui::CollapsingHeader("Metropolis RtoR")) return;

    DragUIntLike("Steps##metropolis", MetropolisCfg.Steps, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 100.0f, "Number of Metropolis updates.");
    ImGui::TextDisabled("GL monitor shows phi(x), pi(x), and cursor telemetry.");

    const char *runTooltip = bStartWithMonitor
        ? "Run and open the passive GL monitor for this simulation."
        : "Run this simulation without opening a GL monitor window.";
    if (ButtonWithTooltip("Run##metropolis", runTooltip)) {
        try { LaunchMetropolis(bStartWithMonitor); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::DrawXYSection() -> void {
    if (!ImGui::CollapsingHeader("XY Metropolis")) return;

    DragUIntLike("Steps##xy", XYCfg.Steps, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f, "Number of Monte Carlo sweeps.");
    DragUIntLike("L##xy", XYCfg.L, 2u, 8192u, 1.0f, "Lattice size (LxL).");
    DragDevFloat("Temperature##xy", XYCfg.Temperature, 0.005, 0.0, 1e6, "%.6g", "Monte Carlo temperature.");
    DragDevFloat("h-field##xy", XYCfg.ExternalField, 0.005, -1e6, 1e6, "%.6g", "External field strength.");
    DragDevFloat("delta-theta##xy", XYCfg.DeltaTheta, 0.005, 1e-8, 1e6, "%.6g", "Proposal angle range.");
    CheckboxWithTooltip("Ferromagnetic initial##xy", &XYCfg.bFerromagneticInitial, "Start from aligned spins instead of random phases.");
    CheckboxWithTooltip("Enable live parameter binding##xy", &XYCfg.bEnableLiveParameterBinding, "Bind selected runtime parameters to LiveControl topics.");
    if (XYCfg.bEnableLiveParameterBinding) {
        DragUIntLike("Control sample interval##xy", XYCfg.ControlSampleInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "How often live parameter topics are polled (steps).");
        CheckboxWithTooltip("Publish launcher controls##xy", &bXYPublishLiveParameters, "Publish current temperature/h-field values to XY control topics every frame.");
        ImGui::TextDisabled("Control topic prefix: %s", XYCfg.ControlTopicPrefix.c_str());
    }
    ImGui::TextDisabled("Passive GL monitor shows theta field + MC diagnostics.");

    const char *runTooltip = bStartWithMonitor
        ? "Run and open the passive GL monitor for this simulation."
        : "Run this simulation without opening a GL monitor window.";
    if (ButtonWithTooltip("Run##xy", runTooltip)) {
        try { LaunchXY(bStartWithMonitor); } catch (const std::exception &e) { LastError = e.what(); }
    }

}

auto FSimulationManagerV2::DrawIsingSection() -> void {
    if (!ImGui::CollapsingHeader("Ising Metropolis")) return;

    DragUIntLike("Steps##ising", IsingCfg.Steps, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f, "Number of Monte Carlo sweeps.");
    DragUIntLike("L##ising", IsingCfg.L, 2u, 8192u, 1.0f, "Lattice size (LxL).");
    DragDevFloat("Temperature##ising", IsingCfg.Temperature, 0.005, 0.0, 1e6, "%.6g", "Monte Carlo temperature.");
    DragDevFloat("h-field##ising", IsingCfg.ExternalField, 0.005, -1e6, 1e6, "%.6g", "External magnetic field.");
    CheckboxWithTooltip("Ferromagnetic initial##ising", &IsingCfg.bFerromagneticInitial, "Start from aligned spins instead of random initialization.");
    CheckboxWithTooltip("Enable live parameter binding##ising", &IsingCfg.bEnableLiveParameterBinding, "Bind selected runtime parameters to LiveControl topics.");
    if (IsingCfg.bEnableLiveParameterBinding) {
        DragUIntLike("Control sample interval##ising", IsingCfg.ControlSampleInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "How often live parameter topics are polled (steps).");
        CheckboxWithTooltip("Publish launcher controls##ising", &bIsingPublishLiveParameters, "Publish current temperature/h-field values to Ising control topics every frame.");
        ImGui::TextDisabled("Control topic prefix: %s", IsingCfg.ControlTopicPrefix.c_str());
    }
    ImGui::TextDisabled("Passive GL monitor shows spin field + MC diagnostics.");

    const char *runTooltip = bStartWithMonitor
        ? "Run and open the passive GL monitor for this simulation."
        : "Run this simulation without opening a GL monitor window.";
    if (ButtonWithTooltip("Run##ising", runTooltip)) {
        try { LaunchIsing(bStartWithMonitor); } catch (const std::exception &e) { LastError = e.what(); }
    }

}

auto FSimulationManagerV2::LaunchSPI(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;

    auto cfg = SPICfg;
    ApplyCommonExecutionSettings(cfg, CommonInterval, CommonMonitorInterval, CommonBatch);
    cfg.bEnableGLMonitor = enableMonitor;

    const bool bNeedLiveView = enableMonitor || bPublishLiveDataWhenHeadless;
    auto liveView = GetOrCreateLiveViewIfNeeded(bNeedLiveView, "spi", SPIRunCounter);

    auto recipe = BuildSPIRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("SPI GL monitor requires a live view.");
        AttachMonitorWindowOrThrow(
            BuildSPIPassiveMonitorWindowV2(cfg, liveView),
            "BuildSPIPassiveMonitorWindow");
    }

    LaunchNumericTask(recipe, cfg.Batch, "SPI");
}

auto FSimulationManagerV2::LaunchRtoR(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;

    auto cfg = RtoRCfg;
    ApplyCommonExecutionSettings(cfg, CommonInterval, CommonMonitorInterval, CommonBatch);
    cfg.bEnableGLMonitor = enableMonitor;
    FinalizeRtoRPlaneWavesExecutionConfigV2(cfg);

    const bool bNeedLiveView = enableMonitor || bPublishLiveDataWhenHeadless;
    auto liveView = GetOrCreateLiveViewIfNeeded(bNeedLiveView, "rtor", RtoRRunCounter);

    auto recipe = BuildRtoRPlaneWavesRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("RtoR GL monitor requires a live view.");
        AttachMonitorWindowOrThrow(
            BuildRtoRPlaneWavesPassiveMonitorWindowV2(cfg, liveView),
            "BuildRtoRPlaneWavesPassiveMonitorWindow");
    }

    LaunchNumericTask(recipe, cfg.Batch, "KGRtoR Plane Waves");
}

auto FSimulationManagerV2::LaunchR2toR(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;

    auto cfg = R2toRCfg;
    ApplyCommonExecutionSettings(cfg, CommonInterval, CommonMonitorInterval, CommonBatch);
    cfg.bEnableGLMonitor = enableMonitor;
    cfg.ControlHub = LiveControlHub;
    FinalizeR2toRBaselineExecutionConfigV2(cfg);

    const bool bNeedLiveView = enableMonitor || bPublishLiveDataWhenHeadless;
    auto liveView = GetOrCreateLiveViewIfNeeded(bNeedLiveView, "kg2d", R2toRRunCounter);

    auto recipe = BuildR2toRBaselineRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("KGR2toR GL monitor requires a live view.");
        AttachMonitorWindowOrThrow(
            BuildR2toRBaselinePassiveMonitorWindowV2(cfg, liveView),
            "BuildR2toRBaselinePassiveMonitorWindow");
    }

    LaunchNumericTask(recipe, cfg.Batch, "KGR2toR");
}

auto FSimulationManagerV2::LaunchMolecularDynamics(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;

    auto cfg = MolecularDynamicsCfg;
    ApplyCommonExecutionSettings(cfg, CommonInterval, CommonMonitorInterval, CommonBatch);
    cfg.bEnableGLMonitor = enableMonitor;
    FinalizeMolecularDynamicsExecutionConfigV2(cfg);

    const bool bNeedLiveView = enableMonitor || bPublishLiveDataWhenHeadless;
    auto liveView = GetOrCreateLiveViewIfNeeded(bNeedLiveView, "moldyn", MolecularDynamicsRunCounter);

    auto recipe = BuildMolecularDynamicsRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("MolecularDynamics GL monitor requires a live view.");
        AttachMonitorWindowOrThrow(
            BuildMolecularDynamicsPassiveMonitorWindowV2(cfg, liveView),
            "BuildMolecularDynamicsPassiveMonitorWindow");
    }

    LaunchNumericTask(recipe, cfg.Batch, "Molecular Dynamics");
}

auto FSimulationManagerV2::LaunchMetropolis(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;

    auto cfg = MetropolisCfg;
    ApplyCommonExecutionSettings(cfg, CommonInterval, CommonMonitorInterval, CommonBatch);
    cfg.bEnableGLMonitor = enableMonitor;
    FinalizeMetropolisExecutionConfigV2(cfg);

    if (enableMonitor) {
        const auto bundle = BuildMetropolisMonitorBundleV2(cfg);
        if (bundle.Recipe == nullptr || bundle.MonitorWindow == nullptr) {
            throw Exception("Lab failed to build Metropolis monitor bundle.");
        }
        AttachMonitorWindowOrThrow(bundle.MonitorWindow, "BuildMetropolisMonitorBundle");
        LaunchNumericTask(bundle.Recipe, cfg.Batch, "Metropolis RtoR");
        return;
    }

    auto recipe = BuildMetropolisRecipeV2(cfg);
    LaunchNumericTask(recipe, cfg.Batch, "Metropolis RtoR");
}

auto FSimulationManagerV2::LaunchXY(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;

    auto cfg = XYCfg;
    ApplyCommonExecutionSettings(cfg, CommonInterval, CommonMonitorInterval, CommonBatch);
    cfg.bEnableGLMonitor = enableMonitor;
    cfg.ControlHub = LiveControlHub;
    FinalizeXYExecutionConfigV2(cfg);

    const bool bNeedLiveView = enableMonitor || bPublishLiveDataWhenHeadless;
    auto liveView = GetOrCreateLiveViewIfNeeded(bNeedLiveView, "xy", XYRunCounter);

    auto recipe = BuildXYRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("XY GL monitor requires a live view.");
        AttachMonitorWindowOrThrow(
            BuildXYPassiveMonitorWindowV2(cfg, liveView),
            "BuildXYPassiveMonitorWindow");
    }

    LaunchNumericTask(recipe, cfg.Batch, "XY Metropolis");
}

auto FSimulationManagerV2::LaunchIsing(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;

    auto cfg = IsingCfg;
    ApplyCommonExecutionSettings(cfg, CommonInterval, CommonMonitorInterval, CommonBatch);
    cfg.bEnableGLMonitor = enableMonitor;
    cfg.ControlHub = LiveControlHub;
    FinalizeIsingExecutionConfigV2(cfg);

    const bool bNeedLiveView = enableMonitor || bPublishLiveDataWhenHeadless;
    auto liveView = GetOrCreateLiveViewIfNeeded(bNeedLiveView, "ising", IsingRunCounter);

    auto recipe = BuildIsingRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("Ising GL monitor requires a live view.");
        AttachMonitorWindowOrThrow(
            BuildIsingPassiveMonitorWindowV2(cfg, liveView),
            "BuildIsingPassiveMonitorWindow");
    }

    LaunchNumericTask(recipe, cfg.Batch, "Ising Metropolis");
}

auto FSimulationManagerV2::MakeTopicName(const Slab::Str &prefix, Slab::UIntBig &counter) -> Slab::Str {
    ++counter;
    return "lab/" + prefix + "/run-" + Slab::ToStr(counter);
}

auto FSimulationManagerV2::LaunchNumericTask(
    const Slab::TPointer<Slab::Math::Numerics::V2::FSimulationRecipeV2> &recipe,
    const Slab::UIntBig batch,
    const Slab::Str &taskNameHint) -> void {
    if (recipe == nullptr) throw Exception("Lab cannot launch null recipe.");

    const auto taskManager = Slab::Core::GetModule<Slab::Core::FTaskManager>("TaskManager");
    if (taskManager == nullptr) throw Exception("TaskManager module not available.");

    auto task = Slab::New<Slab::Math::Numerics::V2::FNumericTaskV2>(recipe, false, static_cast<size_t>(batch));
    Slab::Core::Log::Info() << "Lab launching task: " << taskNameHint << Slab::Core::Log::Flush;
    taskManager->AddTask(task);
}
