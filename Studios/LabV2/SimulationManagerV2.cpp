#include "SimulationManagerV2.h"
#include "StudioConfigV2.h"

#include "imgui.h"

#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Core/SlabCore.h"
#include "Core/Tools/Log.h"

#include "Graphics/Window/WindowStyles.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Listeners/SessionLiveViewPublisherListenerV2.h"
#include "Math/Numerics/V2/Runtime/AppendedSubscriptionsRecipeV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/KleinGordon/RtoR-Montecarlo/V2/RtoR-Hamiltonian-MetropolisHastings-RecipeV2.h"

#include <algorithm>
#include <type_traits>
#include <utility>

namespace {

    template<typename TInt>
    auto DragUIntLike(const char *label, TInt &value, const TInt minVal, const TInt maxVal, const float speed = 1.0f)
        -> bool {
        static_assert(std::is_integral_v<TInt>);
        long long tmp = static_cast<long long>(value);
        const bool changed = ImGui::DragScalar(label, ImGuiDataType_S64, &tmp, speed, nullptr, nullptr);
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
                      const char *format = "%.6g") -> bool {
        return ImGui::DragScalar(label, ImGuiDataType_Double, &value, speed, &minVal, &maxVal, format);
    }

} // namespace

FSimulationManagerV2::FSimulationManagerV2(
    Slab::TPointer<Slab::Graphics::FImGuiContext> imGuiContext,
    Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2> liveDataHub,
    Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2> liveControlHub,
    FAddWindowFn addWindowFn)
: ImGuiContext(std::move(imGuiContext))
, LiveDataHub(std::move(liveDataHub))
, LiveControlHub(std::move(liveControlHub))
, AddWindow(std::move(addWindowFn)) {
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
    R2toRCfg.ControlTopicPrefix = "labv2/control/kg2d";
    R2toRCfg.ForcingWidth = 0.35;
    R2toRCfg.ForcingAmplitude = 0.0;
    R2toRCfg.bForcingEnabled = false;

    MetropolisCfg.Interval = 1000;
    MetropolisCfg.LiveViewInterval = 200;
    MetropolisCfg.Batch = 2048;
}

auto FSimulationManagerV2::NotifyRender(const Slab::Graphics::FPlatformWindow &platformWindow) -> bool {
    AddMenus(platformWindow);
    DrawLauncherWindow();
    return FPlatformWindowEventListener::NotifyRender(platformWindow);
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
                    bShowLauncherWindow = true;
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
                {"Metropolis RtoR (Headless)"},
                {Slab::Graphics::MainMenuSeparator},
                {"Open Launcher"}
            },
            [this, &platformWindow](const Slab::Str &itemString) {
                (void) platformWindow;
                try {
                    if (itemString == "SPI (Headless)") { LaunchSPI(false); return; }
                    if (itemString == "SPI (GL Monitor)") { LaunchSPI(true); return; }
                    if (itemString == "KGRtoR Plane Waves (Headless)") { LaunchRtoR(false); return; }
                    if (itemString == "KGRtoR Plane Waves (GL Monitor)") { LaunchRtoR(true); return; }
                    if (itemString == "KGR2toR Baseline (Headless)") { LaunchR2toR(false); return; }
                    if (itemString == "KGR2toR Baseline (GL Monitor)") { LaunchR2toR(true); return; }
                    if (itemString == "Metropolis RtoR (Headless)") { LaunchMetropolis(); return; }
                    if (itemString == "Open Launcher") { bShowLauncherWindow = true; return; }
                } catch (const std::exception &e) {
                    LastError = e.what();
                    Slab::Core::Log::Error() << "LabV2 launch failed: " << e.what() << Slab::Core::Log::Flush;
                }
            }
        };
        ImGuiContext->AddMainMenuItem(item);
    }
}

auto FSimulationManagerV2::DrawLauncherWindow() -> void {
    if (!bShowLauncherWindow) return;

    ImGui::SetNextWindowPos(
        ImVec2(
            static_cast<float>(FStudioConfigV2::SidePaneWidth + 24),
            static_cast<float>(Slab::Graphics::WindowStyle::GlobalMenuHeight + 24)),
        ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(520, 560), ImGuiCond_Appearing);

    bool bOpen = true;
    if (ImGui::Begin("Lab V2 - Simulation Launcher", &bOpen)) {
        ImGui::TextDisabled("V2 examples launcher (Phase 2/3)");
        if (!LastError.empty()) {
            ImGui::SeparatorText("Last Error");
            ImGui::TextWrapped("%s", LastError.c_str());
            if (ImGui::Button("Clear Error")) LastError.clear();
        }

        DrawSPISection();
        DrawRtoRSection();
        DrawR2toRSection();
        DrawMetropolisSection();
    }
    ImGui::End();

    bShowLauncherWindow = bOpen;
}

auto FSimulationManagerV2::DrawSPISection() -> void {
    if (!ImGui::CollapsingHeader("SPI (V2)", ImGuiTreeNodeFlags_DefaultOpen)) return;

    DragUIntLike("SPI Steps", SPICfg.Steps, 1u, 100000000u, 1.0f);
    DragDevFloat("SPI dT", SPICfg.Dt, 0.0005, 1e-6, 100.0);
    DragDevFloat("SPI Time", SPICfg.Time, 0.01, 1e-6, 1e6);
    DragDevFloat("SPI L", SPICfg.L, 0.01, 1e-6, 1e6);
    DragUIntLike("SPI N", SPICfg.N, 2u, 200000u, 1.0f);
    DragUIntLike("SPI Interval", SPICfg.Interval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f);
    DragUIntLike("SPI Monitor Interval", SPICfg.MonitorInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f);
    DragUIntLike("SPI Batch", SPICfg.Batch, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f);
    ImGui::Checkbox("Publish LiveData when headless##spi", &bSPIPublishLiveViewHeadless);

    if (ImGui::Button("Run SPI Headless")) {
        try { LaunchSPI(false); } catch (const std::exception &e) { LastError = e.what(); }
    }
    ImGui::SameLine();
    if (ImGui::Button("Run SPI + GL Monitor")) {
        try { LaunchSPI(true); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::DrawRtoRSection() -> void {
    if (!ImGui::CollapsingHeader("KGRtoR Plane Waves (V2)", ImGuiTreeNodeFlags_DefaultOpen)) return;

    DragUIntLike("RtoR Steps", RtoRCfg.Steps, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f);

    bool bUseDtOverride = RtoRCfg.Dt > 0.0;
    if (ImGui::Checkbox("Override dt (otherwise legacy dt/h=0.1)", &bUseDtOverride)) {
        if (!bUseDtOverride) RtoRCfg.Dt = -1.0;
        else if (RtoRCfg.Dt <= 0.0) RtoRCfg.Dt = 0.01;
    }
    if (bUseDtOverride) {
        DragDevFloat("RtoR dT", RtoRCfg.Dt, 0.0001, 1e-8, 100.0);
    } else {
        ImGui::TextDisabled("dt auto = 0.1 * (L / N)");
    }

    DragDevFloat("RtoR L", RtoRCfg.L, 0.01, 1e-6, 1e6);
    DragUIntLike("RtoR N", RtoRCfg.N, 4u, 200000u, 1.0f);
    DragDevFloat("RtoR x-center", RtoRCfg.XCenter, 0.01, -1e6, 1e6);
    DragDevFloat("RtoR Q", RtoRCfg.Q, 0.01, -1e6, 1e6);
    DragUIntLike("RtoR harmonic", RtoRCfg.Harmonic, 1u, 100000u, 1.0f);
    DragUIntLike("RtoR Interval", RtoRCfg.Interval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f);
    DragUIntLike("RtoR Monitor Interval", RtoRCfg.MonitorInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f);
    DragUIntLike("RtoR Batch", RtoRCfg.Batch, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f);

    ImGui::Checkbox("Publish LiveData when headless##rtor", &bRtoRPublishLiveViewHeadless);
    ImGui::TextDisabled("Advanced analysis listeners are currently exposed via CLI (Studios rtor).");

    if (ImGui::Button("Run RtoR Headless")) {
        try { LaunchRtoR(false); } catch (const std::exception &e) { LastError = e.what(); }
    }
    ImGui::SameLine();
    if (ImGui::Button("Run RtoR + GL Monitor")) {
        try { LaunchRtoR(true); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::DrawR2toRSection() -> void {
    if (!ImGui::CollapsingHeader("KGR2toR Baseline (V2)", ImGuiTreeNodeFlags_DefaultOpen)) return;

    DragUIntLike("KG2D Steps", R2toRCfg.Steps, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f);
    DragDevFloat("KG2D L", R2toRCfg.L, 0.01, 1e-6, 1e6);
    DragUIntLike("KG2D N", R2toRCfg.N, 8u, 200000u, 1.0f);
    DragDevFloat("KG2D r_dt", R2toRCfg.RDt, 0.001, 1e-8, 10.0, "%.6g");
    ImGui::TextDisabled("dt auto = r_dt * (L / N)");

    DragDevFloat("KG2D x-center", R2toRCfg.XCenter, 0.01, -1e6, 1e6);
    DragDevFloat("KG2D y-center", R2toRCfg.YCenter, 0.01, -1e6, 1e6);
    DragDevFloat("KG2D pulse width", R2toRCfg.PulseWidth, 0.001, 1e-8, 1e6, "%.6g");
    DragDevFloat("KG2D phi amplitude", R2toRCfg.PhiAmplitude, 0.01, -1e6, 1e6);
    DragDevFloat("KG2D dphi/dt amplitude", R2toRCfg.DPhiDtAmplitude, 0.01, -1e6, 1e6);
    DragUIntLike("KG2D Interval", R2toRCfg.Interval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f);
    DragUIntLike("KG2D Monitor Interval", R2toRCfg.MonitorInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f);
    DragUIntLike("KG2D Batch", R2toRCfg.Batch, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f);
    ImGui::Checkbox("Enable LiveControl forcing binding##kg2d-bind", &R2toRCfg.bEnableLiveControlForcing);
    if (R2toRCfg.bEnableLiveControlForcing) {
        DragUIntLike("KG2D control sample interval", R2toRCfg.ControlSampleInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 1.0f);
        DragDevFloat("KG2D forcing x", R2toRCfg.ForcingXCenter, 0.01, -1e6, 1e6);
        DragDevFloat("KG2D forcing y", R2toRCfg.ForcingYCenter, 0.01, -1e6, 1e6);
        DragDevFloat("KG2D forcing width", R2toRCfg.ForcingWidth, 0.001, 1e-8, 1e6, "%.6g");
        DragDevFloat("KG2D forcing amplitude", R2toRCfg.ForcingAmplitude, 0.01, -1e6, 1e6);
        ImGui::Checkbox("KG2D forcing enabled", &R2toRCfg.bForcingEnabled);
        ImGui::TextDisabled("Control topic prefix: %s", R2toRCfg.ControlTopicPrefix.c_str());
    }

    ImGui::Checkbox("Publish LiveData when headless##kg2d", &bR2toRPublishLiveViewHeadless);

    if (ImGui::Button("Run KG2D Headless")) {
        try { LaunchR2toR(false); } catch (const std::exception &e) { LastError = e.what(); }
    }
    ImGui::SameLine();
    if (ImGui::Button("Run KG2D + GL Monitor")) {
        try { LaunchR2toR(true); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::DrawMetropolisSection() -> void {
    if (!ImGui::CollapsingHeader("Metropolis RtoR (V2)")) return;

    DragUIntLike("Metropolis Steps", MetropolisCfg.Steps, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 100.0f);
    DragUIntLike("Metropolis Interval", MetropolisCfg.Interval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f);
    DragUIntLike("Metropolis LiveView Interval", MetropolisCfg.LiveViewInterval, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f);
    DragUIntLike("Metropolis Batch", MetropolisCfg.Batch, Slab::UIntBig(1), Slab::UIntBig(1ull << 40), 10.0f);
    ImGui::Checkbox("Publish LiveData##metro", &MetropolisCfg.bPublishLiveView);
    ImGui::TextDisabled("No passive GL monitor yet (headless + console/live telemetry only).");

    if (ImGui::Button("Run Metropolis Headless")) {
        try { LaunchMetropolis(); } catch (const std::exception &e) { LastError = e.what(); }
    }
}

auto FSimulationManagerV2::LaunchSPI(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;

    auto cfg = SPICfg;
    cfg.bEnableGLMonitor = enableMonitor;

    const bool bNeedLiveView = enableMonitor || bSPIPublishLiveViewHeadless;
    Slab::TPointer<Slab::Math::LiveData::V2::FSessionLiveViewV2> liveView = nullptr;
    if (bNeedLiveView && LiveDataHub != nullptr) {
        liveView = LiveDataHub->GetOrCreateSessionLiveView(MakeTopicName("spi", SPIRunCounter));
    }

    auto recipe = BuildSPIRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("SPI GL monitor requires a live view.");
        if (!AddWindow) throw Exception("LabV2 cannot attach monitor window.");
        AddWindow(BuildSPIPassiveMonitorWindowV2(cfg, liveView));
    }

    LaunchNumericTask(recipe, cfg.Batch, "SPI V2");
}

auto FSimulationManagerV2::LaunchRtoR(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;

    auto cfg = RtoRCfg;
    cfg.bEnableGLMonitor = enableMonitor;
    FinalizeRtoRPlaneWavesExecutionConfigV2(cfg);

    const bool bNeedLiveView = enableMonitor || bRtoRPublishLiveViewHeadless;
    Slab::TPointer<Slab::Math::LiveData::V2::FSessionLiveViewV2> liveView = nullptr;
    if (bNeedLiveView && LiveDataHub != nullptr) {
        liveView = LiveDataHub->GetOrCreateSessionLiveView(MakeTopicName("rtor", RtoRRunCounter));
    }

    auto recipe = BuildRtoRPlaneWavesRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("RtoR GL monitor requires a live view.");
        if (!AddWindow) throw Exception("LabV2 cannot attach monitor window.");
        AddWindow(BuildRtoRPlaneWavesPassiveMonitorWindowV2(cfg, liveView));
    }

    LaunchNumericTask(recipe, cfg.Batch, "KGRtoR Plane Waves V2");
}

auto FSimulationManagerV2::LaunchR2toR(const bool enableMonitor) -> void {
    using namespace Slab::Studios::Common::Simulations::V2;

    auto cfg = R2toRCfg;
    cfg.bEnableGLMonitor = enableMonitor;
    cfg.ControlHub = LiveControlHub;
    FinalizeR2toRBaselineExecutionConfigV2(cfg);

    const bool bNeedLiveView = enableMonitor || bR2toRPublishLiveViewHeadless;
    Slab::TPointer<Slab::Math::LiveData::V2::FSessionLiveViewV2> liveView = nullptr;
    if (bNeedLiveView && LiveDataHub != nullptr) {
        liveView = LiveDataHub->GetOrCreateSessionLiveView(MakeTopicName("kg2d", R2toRRunCounter));
    }

    auto recipe = BuildR2toRBaselineRecipeV2(cfg, liveView);
    if (enableMonitor) {
        if (liveView == nullptr) throw Exception("KGR2toR GL monitor requires a live view.");
        if (!AddWindow) throw Exception("LabV2 cannot attach monitor window.");
        AddWindow(BuildR2toRBaselinePassiveMonitorWindowV2(cfg, liveView));
    }

    LaunchNumericTask(recipe, cfg.Batch, "KGR2toR Baseline V2");
}

auto FSimulationManagerV2::LaunchMetropolis() -> void {
    using namespace Slab::Math::Numerics::V2;
    using namespace Slab::Models::KGRtoR::Metropolis::V2;

    Slab::TPointer<FSimulationRecipeV2> recipe = Slab::New<FRtoRHamiltonianMetropolisHastingsRecipeV2>(
        MetropolisCfg.Steps,
        std::max<Slab::UIntBig>(Slab::UIntBig(1), MetropolisCfg.Interval));

    if (MetropolisCfg.bPublishLiveView && LiveDataHub != nullptr) {
        auto liveView = LiveDataHub->GetOrCreateSessionLiveView(MakeTopicName("metropolis", MetropolisRunCounter));
        Slab::Vector<FSubscriptionV2> extras = {{
            Slab::New<FEveryNStepsTriggerV2>(std::max<Slab::UIntBig>(Slab::UIntBig(1), MetropolisCfg.LiveViewInterval)),
            Slab::New<FSessionLiveViewPublisherListenerV2>(liveView, "Metropolis Live View Publisher V2"),
            EDeliveryModeV2::Synchronous,
            true,
            true
        }};
        recipe = Slab::New<FAppendedSubscriptionsRecipeV2>(recipe, std::move(extras));
    }

    LaunchNumericTask(recipe, MetropolisCfg.Batch, "Metropolis RtoR V2");
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
    (void) taskNameHint;
    taskManager->AddTask(task);
}
