#include "SimulationManagerV2.h"

#include "imgui.h"

#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Core/SlabCore.h"
#include "Core/Tools/Log.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Listeners/StateSnapshotListenerV2.h"
#include "Math/Numerics/V2/Runtime/AppendedSubscriptionsRecipeV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"
#include "Studios/Common/Simulations/V2/KGR2toRControlTopicsV2.h"

#include <algorithm>
#include <array>
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
    SPICfg.Interval = 10;
    SPICfg.MonitorInterval = 2;
    SPICfg.Batch = 512;

    RtoRCfg.Interval = 20;
    RtoRCfg.MonitorInterval = 5;
    RtoRCfg.Batch = 256;
    RtoRCfg.bHistorySummary = true;

    R2toRCfg.Interval = 20;
    R2toRCfg.MonitorInterval = 5;
    R2toRCfg.Batch = 256;
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
    MolecularDynamicsCfg.Interval = 20;
    MolecularDynamicsCfg.MonitorInterval = 5;
    MolecularDynamicsCfg.Batch = 256;

    XYCfg.L = 64;
    XYCfg.Steps = 2000;
    XYCfg.Temperature = 0.7;
    XYCfg.ExternalField = 0.0;
    XYCfg.DeltaTheta = 2.0 * std::numbers::pi_v<Slab::DevFloat>;
    XYCfg.Interval = 100;
    XYCfg.MonitorInterval = 20;
    XYCfg.Batch = 1024;

    IsingCfg.L = 64;
    IsingCfg.Steps = 2000;
    IsingCfg.Temperature = 2.269185314;
    IsingCfg.ExternalField = 0.0;
    IsingCfg.Interval = 100;
    IsingCfg.MonitorInterval = 20;
    IsingCfg.Batch = 1024;

    MetropolisCfg.Interval = 1000;
    MetropolisCfg.MonitorInterval = 100;
    MetropolisCfg.Batch = 2048;
}

auto FSimulationManagerV2::AddMenus(const Slab::Graphics::FPlatformWindow &platformWindow) -> void {
    if (ImGuiContext == nullptr) return;

    {
        const auto itemLocation = Slab::Graphics::MainMenuLocation{"Lab V2"};
        const auto item = Slab::Graphics::MainMenuItem{
            itemLocation,
            {
                {"Launcher", "Show V2 simulation launcher"}
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
        const auto itemLocation = Slab::Graphics::MainMenuLocation{"Simulations", "V2"};
        const auto item = Slab::Graphics::MainMenuItem{
            itemLocation,
            {
                {"SPI (Headless)"},
                {"SPI (GL Monitor)"},
                {"KGRtoR Plane Waves (Headless)"},
                {"KGRtoR Plane Waves (GL Monitor)"},
                {"KGR2toR Baseline (Headless)"},
                {"KGR2toR Baseline (GL Monitor)"},
                {Slab::Graphics::MainMenuSeparator},
                {"Molecular Dynamics (Headless)"},
                {"Molecular Dynamics (GL Monitor)"},
                {Slab::Graphics::MainMenuSeparator},
                {"XY Metropolis (Headless)"},
                {"XY Metropolis (GL Monitor)"},
                {Slab::Graphics::MainMenuSeparator},
                {"Ising Metropolis (Headless)"},
                {"Ising Metropolis (GL Monitor)"},
                {Slab::Graphics::MainMenuSeparator},
                {"Metropolis RtoR (Headless)"},
                {"Metropolis RtoR (GL Monitor)"},
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
        bool bEnableMonitor;
    };

    static constexpr std::array<FLaunchMenuEntry, 14> entries = {{
        {"SPI (Headless)", &FSimulationManagerV2::LaunchSPI, false},
        {"SPI (GL Monitor)", &FSimulationManagerV2::LaunchSPI, true},
        {"KGRtoR Plane Waves (Headless)", &FSimulationManagerV2::LaunchRtoR, false},
        {"KGRtoR Plane Waves (GL Monitor)", &FSimulationManagerV2::LaunchRtoR, true},
        {"KGR2toR Baseline (Headless)", &FSimulationManagerV2::LaunchR2toR, false},
        {"KGR2toR Baseline (GL Monitor)", &FSimulationManagerV2::LaunchR2toR, true},
        {"Molecular Dynamics (Headless)", &FSimulationManagerV2::LaunchMolecularDynamics, false},
        {"Molecular Dynamics (GL Monitor)", &FSimulationManagerV2::LaunchMolecularDynamics, true},
        {"XY Metropolis (Headless)", &FSimulationManagerV2::LaunchXY, false},
        {"XY Metropolis (GL Monitor)", &FSimulationManagerV2::LaunchXY, true},
        {"Ising Metropolis (Headless)", &FSimulationManagerV2::LaunchIsing, false},
        {"Ising Metropolis (GL Monitor)", &FSimulationManagerV2::LaunchIsing, true},
        {"Metropolis RtoR (Headless)", &FSimulationManagerV2::LaunchMetropolis, false},
        {"Metropolis RtoR (GL Monitor)", &FSimulationManagerV2::LaunchMetropolis, true}
    }};

    for (const auto &entry : entries) {
        if (itemString != entry.Label) continue;
        (this->*entry.LaunchFn)(entry.bEnableMonitor);
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
    Slab::Core::Log::Error() << "LabV2 launch failed: " << e.what() << Slab::Core::Log::Flush;
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
    if (!AddWindow) throw Exception("LabV2 cannot attach monitor window.");
    AddWindow(window);
}

auto FSimulationManagerV2::DrawLauncherContents() -> void {
    ImGui::TextDisabled("Examples launcher");
    if (!LastError.empty()) {
        ImGui::SeparatorText("Last Error");
        ImGui::TextWrapped("%s", LastError.c_str());
        if (ButtonWithTooltip("Clear Error", "Clear the latest launch error message.")) LastError.clear();
    }

    DrawSPISection();
    DrawRtoRSection();
    DrawR2toRSection();
    DrawMolecularDynamicsSection();
    DrawXYSection();
    DrawIsingSection();
    DrawMetropolisSection();
}

auto FSimulationManagerV2::DrawSPISection() -> void {
    if (!ImGui::CollapsingHeader("SPI", ImGuiTreeNodeFlags_DefaultOpen)) return;

    DragUIntLike("Steps##spi", SPICfg.Steps, 1u, 100000000u, 1.0f, "Number of stochastic-time integration steps.");
    DragDevFloat("dT##spi", SPICfg.Dt, 0.0005, 1e-6, 100.0, "%.6g", "Stochastic-time step size.");
    DragDevFloat("Time##spi", SPICfg.Time, 0.01, 1e-6, 1e6, "%.6g", "Physical time horizon.");
    DragDevFloat("L##spi", SPICfg.L, 0.01, 1e-6, 1e6, "%.6g", "Spatial domain length.");
    DragUIntLike("N##spi", SPICfg.N, 2u, 200000u, 1.0f, "Number of lattice sites.");
    DragUIntLike("Interval##spi", SPICfg.Interval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Console/log sampling interval (steps).");
    DragUIntLike("Monitor interval##spi", SPICfg.MonitorInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Snapshot publishing interval for monitor/live data.");
    DragUIntLike("Batch##spi", SPICfg.Batch, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Max internal steps per worker loop iteration.");
    CheckboxWithTooltip("Publish live data when headless##spi", &bSPIPublishLiveViewHeadless, "Expose telemetry/live data even without GL monitor.");

    if (ButtonWithTooltip("Run Headless##spi", "Run this simulation without opening a GL monitor window.")) {
        try { LaunchSPI(false); } catch (const std::exception &e) { LastError = e.what(); }
    }
    ImGui::SameLine();
    if (ButtonWithTooltip("Run + GL Monitor##spi", "Run and open the passive GL monitor for this simulation.")) {
        try { LaunchSPI(true); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::DrawRtoRSection() -> void {
    if (!ImGui::CollapsingHeader("KGRtoR Plane Waves", ImGuiTreeNodeFlags_DefaultOpen)) return;

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
    DragUIntLike("Interval##rtor", RtoRCfg.Interval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Console/log sampling interval (steps).");
    DragUIntLike("Monitor interval##rtor", RtoRCfg.MonitorInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Snapshot publishing interval for monitor/live data.");
    DragUIntLike("Batch##rtor", RtoRCfg.Batch, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Max internal steps per worker loop iteration.");

    CheckboxWithTooltip("Publish live data when headless##rtor", &bRtoRPublishLiveViewHeadless, "Expose telemetry/live data even without GL monitor.");
    ImGui::TextDisabled("Advanced analysis listeners are currently exposed via CLI (Studios rtor).");

    if (ButtonWithTooltip("Run Headless##rtor", "Run this simulation without opening a GL monitor window.")) {
        try { LaunchRtoR(false); } catch (const std::exception &e) { LastError = e.what(); }
    }
    ImGui::SameLine();
    if (ButtonWithTooltip("Run + GL Monitor##rtor", "Run and open the passive GL monitor for this simulation.")) {
        try { LaunchRtoR(true); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::DrawR2toRSection() -> void {
    if (!ImGui::CollapsingHeader("KGR2toR Baseline", ImGuiTreeNodeFlags_DefaultOpen)) return;

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
    DragUIntLike("Interval##kg2d", R2toRCfg.Interval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Console/log sampling interval (steps).");
    DragUIntLike("Monitor interval##kg2d", R2toRCfg.MonitorInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Snapshot publishing interval for monitor/live data.");
    DragUIntLike("Batch##kg2d", R2toRCfg.Batch, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Max internal steps per worker loop iteration.");
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

    CheckboxWithTooltip("Publish live data when headless##kg2d", &bR2toRPublishLiveViewHeadless, "Expose telemetry/live data even without GL monitor.");

    if (ButtonWithTooltip("Run Headless##kg2d", "Run this simulation without opening a GL monitor window.")) {
        try { LaunchR2toR(false); } catch (const std::exception &e) { LastError = e.what(); }
    }
    ImGui::SameLine();
    if (ButtonWithTooltip("Run + GL Monitor##kg2d", "Run and open the passive GL monitor for this simulation.")) {
        try { LaunchR2toR(true); } catch (const std::exception &e) { LastError = e.what(); }
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
    DragUIntLike("Interval##md", MolecularDynamicsCfg.Interval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Console/log sampling interval (steps).");
    DragUIntLike("Monitor interval##md", MolecularDynamicsCfg.MonitorInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Snapshot publishing interval for monitor/live data.");
    DragUIntLike("Batch##md", MolecularDynamicsCfg.Batch, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f, "Max internal steps per worker loop iteration.");
    CheckboxWithTooltip("Publish live data when headless##moldyn", &bMolecularDynamicsPublishLiveViewHeadless, "Expose telemetry/live data even without GL monitor.");

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

    if (ButtonWithTooltip("Run Headless##md", "Run this simulation without opening a GL monitor window.")) {
        try { LaunchMolecularDynamics(false); } catch (const std::exception &e) { LastError = e.what(); }
    }
    ImGui::SameLine();
    if (ButtonWithTooltip("Run + GL Monitor##md", "Run and open the passive GL monitor for this simulation.")) {
        try { LaunchMolecularDynamics(true); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::DrawMetropolisSection() -> void {
    if (!ImGui::CollapsingHeader("Metropolis RtoR")) return;

    DragUIntLike("Steps##metropolis", MetropolisCfg.Steps, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 100.0f, "Number of Metropolis updates.");
    DragUIntLike("Interval##metropolis", MetropolisCfg.Interval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f, "Console/log sampling interval (steps).");
    DragUIntLike("Monitor interval##metropolis", MetropolisCfg.MonitorInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f, "Snapshot publishing interval for monitor/live data.");
    DragUIntLike("Batch##metropolis", MetropolisCfg.Batch, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f, "Max internal steps per worker loop iteration.");
    ImGui::TextDisabled("GL monitor shows phi(x), pi(x), and cursor telemetry.");

    if (ButtonWithTooltip("Run Headless##metropolis", "Run this simulation without opening a GL monitor window.")) {
        try { LaunchMetropolis(false); } catch (const std::exception &e) { LastError = e.what(); }
    }
    ImGui::SameLine();
    if (ButtonWithTooltip("Run + GL Monitor##metropolis", "Run and open the passive GL monitor for this simulation.")) {
        try { LaunchMetropolis(true); } catch (const std::exception &e) { LastError = e.what(); }
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
    DragUIntLike("Interval##xy", XYCfg.Interval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f, "Console/log sampling interval (steps).");
    DragUIntLike("Monitor interval##xy", XYCfg.MonitorInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f, "Snapshot publishing interval for monitor/live data.");
    DragUIntLike("Batch##xy", XYCfg.Batch, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f, "Max internal steps per worker loop iteration.");
    CheckboxWithTooltip("Publish live data when headless##xy", &bXYPublishLiveViewHeadless, "Expose telemetry/live data even without GL monitor.");

    ImGui::TextDisabled("Passive GL monitor shows theta field + MC diagnostics.");

    if (ButtonWithTooltip("Run Headless##xy", "Run this simulation without opening a GL monitor window.")) {
        try { LaunchXY(false); } catch (const std::exception &e) { LastError = e.what(); }
    }
    ImGui::SameLine();
    if (ButtonWithTooltip("Run + GL Monitor##xy", "Run and open the passive GL monitor for this simulation.")) {
        try { LaunchXY(true); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::DrawIsingSection() -> void {
    if (!ImGui::CollapsingHeader("Ising Metropolis")) return;

    DragUIntLike("Steps##ising", IsingCfg.Steps, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f, "Number of Monte Carlo sweeps.");
    DragUIntLike("L##ising", IsingCfg.L, 2u, 8192u, 1.0f, "Lattice size (LxL).");
    DragDevFloat("Temperature##ising", IsingCfg.Temperature, 0.005, 0.0, 1e6, "%.6g", "Monte Carlo temperature.");
    DragDevFloat("h-field##ising", IsingCfg.ExternalField, 0.005, -1e6, 1e6, "%.6g", "External magnetic field.");
    CheckboxWithTooltip("Ferromagnetic initial##ising", &IsingCfg.bFerromagneticInitial, "Start from aligned spins instead of random initialization.");
    DragUIntLike("Interval##ising", IsingCfg.Interval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f, "Console/log sampling interval (steps).");
    DragUIntLike("Monitor interval##ising", IsingCfg.MonitorInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f, "Snapshot publishing interval for monitor/live data.");
    DragUIntLike("Batch##ising", IsingCfg.Batch, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f, "Max internal steps per worker loop iteration.");
    CheckboxWithTooltip("Publish live data when headless##ising", &bIsingPublishLiveViewHeadless, "Expose telemetry/live data even without GL monitor.");

    ImGui::TextDisabled("Passive GL monitor shows spin field + MC diagnostics.");

    if (ButtonWithTooltip("Run Headless##ising", "Run this simulation without opening a GL monitor window.")) {
        try { LaunchIsing(false); } catch (const std::exception &e) { LastError = e.what(); }
    }
    ImGui::SameLine();
    if (ButtonWithTooltip("Run + GL Monitor##ising", "Run and open the passive GL monitor for this simulation.")) {
        try { LaunchIsing(true); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::LaunchSPI(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;
    using namespace Slab::Math::Numerics::V2;

    auto cfg = SPICfg;
    cfg.bEnableGLMonitor = enableMonitor;

    const bool bNeedLiveView = enableMonitor || bSPIPublishLiveViewHeadless;
    auto liveView = GetOrCreateLiveViewIfNeeded(bNeedLiveView, "spi", SPIRunCounter);

    auto recipe = BuildSPIRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("SPI GL monitor requires a live view.");
        auto snapshotListener = Slab::New<FStateSnapshotListenerV2>("LabV2 SPI monitor snapshot listener V2");
        Slab::Vector<FSubscriptionV2> subscriptions = {{
            Slab::New<FEveryNStepsTriggerV2>(std::max<Slab::UIntBig>(Slab::UIntBig(1), cfg.MonitorInterval)),
            snapshotListener,
            EDeliveryModeV2::LatestOnly,
            true,
            true
        }};
        recipe = Slab::New<FAppendedSubscriptionsRecipeV2>(recipe, std::move(subscriptions));
        AttachMonitorWindowOrThrow(
            BuildSPIPassiveMonitorWindowV2(cfg, liveView, snapshotListener),
            "BuildSPIPassiveMonitorWindowV2");
    }

    LaunchNumericTask(recipe, cfg.Batch, "SPI V2");
}

auto FSimulationManagerV2::LaunchRtoR(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;
    using namespace Slab::Math::Numerics::V2;

    auto cfg = RtoRCfg;
    cfg.bEnableGLMonitor = enableMonitor;
    FinalizeRtoRPlaneWavesExecutionConfigV2(cfg);

    const bool bNeedLiveView = enableMonitor || bRtoRPublishLiveViewHeadless;
    auto liveView = GetOrCreateLiveViewIfNeeded(bNeedLiveView, "rtor", RtoRRunCounter);

    auto recipe = BuildRtoRPlaneWavesRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("RtoR GL monitor requires a live view.");
        auto snapshotListener = Slab::New<FStateSnapshotListenerV2>("LabV2 KGRtoR monitor snapshot listener V2");
        Slab::Vector<FSubscriptionV2> subscriptions = {{
            Slab::New<FEveryNStepsTriggerV2>(std::max<Slab::UIntBig>(Slab::UIntBig(1), cfg.MonitorInterval)),
            snapshotListener,
            EDeliveryModeV2::LatestOnly,
            true,
            true
        }};
        recipe = Slab::New<FAppendedSubscriptionsRecipeV2>(recipe, std::move(subscriptions));
        AttachMonitorWindowOrThrow(
            BuildRtoRPlaneWavesPassiveMonitorWindowV2(cfg, liveView, snapshotListener),
            "BuildRtoRPlaneWavesPassiveMonitorWindowV2");
    }

    LaunchNumericTask(recipe, cfg.Batch, "KGRtoR Plane Waves V2");
}

auto FSimulationManagerV2::LaunchR2toR(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;
    using namespace Slab::Math::Numerics::V2;

    auto cfg = R2toRCfg;
    cfg.bEnableGLMonitor = enableMonitor;
    cfg.ControlHub = LiveControlHub;
    FinalizeR2toRBaselineExecutionConfigV2(cfg);

    const bool bNeedLiveView = enableMonitor || bR2toRPublishLiveViewHeadless;
    auto liveView = GetOrCreateLiveViewIfNeeded(bNeedLiveView, "kg2d", R2toRRunCounter);

    auto recipe = BuildR2toRBaselineRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("KGR2toR GL monitor requires a live view.");
        auto snapshotListener = Slab::New<FStateSnapshotListenerV2>("LabV2 KGR2toR monitor snapshot listener V2");
        Slab::Vector<FSubscriptionV2> subscriptions = {{
            Slab::New<FEveryNStepsTriggerV2>(std::max<Slab::UIntBig>(Slab::UIntBig(1), cfg.MonitorInterval)),
            snapshotListener,
            EDeliveryModeV2::LatestOnly,
            true,
            true
        }};
        recipe = Slab::New<FAppendedSubscriptionsRecipeV2>(recipe, std::move(subscriptions));
        AttachMonitorWindowOrThrow(
            BuildR2toRBaselinePassiveMonitorWindowV2(cfg, liveView, snapshotListener),
            "BuildR2toRBaselinePassiveMonitorWindowV2");
    }

    LaunchNumericTask(recipe, cfg.Batch, "KGR2toR Baseline V2");
}

auto FSimulationManagerV2::LaunchMolecularDynamics(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;
    using namespace Slab::Math::Numerics::V2;

    auto cfg = MolecularDynamicsCfg;
    cfg.bEnableGLMonitor = enableMonitor;
    FinalizeMolecularDynamicsExecutionConfigV2(cfg);

    const bool bNeedLiveView = enableMonitor || bMolecularDynamicsPublishLiveViewHeadless;
    auto liveView = GetOrCreateLiveViewIfNeeded(bNeedLiveView, "moldyn", MolecularDynamicsRunCounter);

    auto recipe = BuildMolecularDynamicsRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("MolecularDynamics GL monitor requires a live view.");
        auto snapshotListener = Slab::New<FStateSnapshotListenerV2>(
            "LabV2 MolecularDynamics monitor snapshot listener V2");
        Slab::Vector<FSubscriptionV2> subscriptions = {{
            Slab::New<FEveryNStepsTriggerV2>(std::max<Slab::UIntBig>(Slab::UIntBig(1), cfg.MonitorInterval)),
            snapshotListener,
            EDeliveryModeV2::LatestOnly,
            true,
            true
        }};
        recipe = Slab::New<FAppendedSubscriptionsRecipeV2>(recipe, std::move(subscriptions));
        AttachMonitorWindowOrThrow(
            BuildMolecularDynamicsPassiveMonitorWindowV2(cfg, liveView, snapshotListener),
            "BuildMolecularDynamicsPassiveMonitorWindowV2");
    }

    LaunchNumericTask(recipe, cfg.Batch, "Molecular Dynamics V2");
}

auto FSimulationManagerV2::LaunchMetropolis(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;

    auto cfg = MetropolisCfg;
    cfg.bEnableGLMonitor = enableMonitor;
    FinalizeMetropolisExecutionConfigV2(cfg);

    if (enableMonitor) {
        const auto bundle = BuildMetropolisMonitorBundleV2(cfg);
        if (bundle.Recipe == nullptr || bundle.MonitorWindow == nullptr) {
            throw Exception("LabV2 failed to build Metropolis monitor bundle.");
        }
        AttachMonitorWindowOrThrow(bundle.MonitorWindow, "BuildMetropolisMonitorBundleV2");
        LaunchNumericTask(bundle.Recipe, cfg.Batch, "Metropolis RtoR V2");
        return;
    }

    auto recipe = BuildMetropolisRecipeV2(cfg);
    LaunchNumericTask(recipe, cfg.Batch, "Metropolis RtoR V2");
}

auto FSimulationManagerV2::LaunchXY(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;
    using namespace Slab::Math::Numerics::V2;

    auto cfg = XYCfg;
    cfg.bEnableGLMonitor = enableMonitor;
    FinalizeXYExecutionConfigV2(cfg);

    const bool bNeedLiveView = enableMonitor || bXYPublishLiveViewHeadless;
    auto liveView = GetOrCreateLiveViewIfNeeded(bNeedLiveView, "xy", XYRunCounter);

    auto recipe = BuildXYRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("XY GL monitor requires a live view.");
        auto snapshotListener = Slab::New<FStateSnapshotListenerV2>("LabV2 XY monitor snapshot listener V2");
        Slab::Vector<FSubscriptionV2> subscriptions = {{
            Slab::New<FEveryNStepsTriggerV2>(std::max<Slab::UIntBig>(Slab::UIntBig(1), cfg.MonitorInterval)),
            snapshotListener,
            EDeliveryModeV2::LatestOnly,
            true,
            true
        }};
        recipe = Slab::New<FAppendedSubscriptionsRecipeV2>(recipe, std::move(subscriptions));
        AttachMonitorWindowOrThrow(
            BuildXYPassiveMonitorWindowV2(cfg, liveView, snapshotListener),
            "BuildXYPassiveMonitorWindowV2");
    }

    LaunchNumericTask(recipe, cfg.Batch, "XY Metropolis V2");
}

auto FSimulationManagerV2::LaunchIsing(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;
    using namespace Slab::Math::Numerics::V2;

    auto cfg = IsingCfg;
    cfg.bEnableGLMonitor = enableMonitor;
    FinalizeIsingExecutionConfigV2(cfg);

    const bool bNeedLiveView = enableMonitor || bIsingPublishLiveViewHeadless;
    auto liveView = GetOrCreateLiveViewIfNeeded(bNeedLiveView, "ising", IsingRunCounter);

    auto recipe = BuildIsingRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("Ising GL monitor requires a live view.");
        auto snapshotListener = Slab::New<FStateSnapshotListenerV2>("LabV2 Ising monitor snapshot listener V2");
        Slab::Vector<FSubscriptionV2> subscriptions = {{
            Slab::New<FEveryNStepsTriggerV2>(std::max<Slab::UIntBig>(Slab::UIntBig(1), cfg.MonitorInterval)),
            snapshotListener,
            EDeliveryModeV2::LatestOnly,
            true,
            true
        }};
        recipe = Slab::New<FAppendedSubscriptionsRecipeV2>(recipe, std::move(subscriptions));
        AttachMonitorWindowOrThrow(
            BuildIsingPassiveMonitorWindowV2(cfg, liveView, snapshotListener),
            "BuildIsingPassiveMonitorWindowV2");
    }

    LaunchNumericTask(recipe, cfg.Batch, "Ising Metropolis V2");
}

auto FSimulationManagerV2::MakeTopicName(const Slab::Str &prefix, Slab::UIntBig &counter) -> Slab::Str {
    ++counter;
    return "labv2/" + prefix + "/run-" + Slab::ToStr(counter);
}

auto FSimulationManagerV2::LaunchNumericTask(
    const Slab::TPointer<Slab::Math::Numerics::V2::FSimulationRecipeV2> &recipe,
    const Slab::UIntBig batch,
    const Slab::Str &taskNameHint) -> void {
    if (recipe == nullptr) throw Exception("LabV2 cannot launch null V2 recipe.");

    const auto taskManager = Slab::Core::GetModule<Slab::Core::FTaskManager>("TaskManager");
    if (taskManager == nullptr) throw Exception("TaskManager module not available.");

    auto task = Slab::New<Slab::Math::Numerics::V2::FNumericTaskV2>(recipe, false, static_cast<size_t>(batch));
    Slab::Core::Log::Info() << "LabV2 launching task: " << taskNameHint << Slab::Core::Log::Flush;
    taskManager->AddTask(task);
}
