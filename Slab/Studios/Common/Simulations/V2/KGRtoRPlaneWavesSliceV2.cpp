#include "KGRtoRPlaneWavesSliceV2.h"

#include "../../Monitors/V2/KGRtoRPlaneWavesPassiveMonitorWindowV2.h"
#include "../../NumericsV2TaskUtils.h"
#include "../../V2SimulationRunners.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/KleinGordon/RtoR/LinearStepping/V2/KG-RtoR-PlaneWaves-RecipeV2.h"

namespace Slab::Studios::Common::Simulations::V2 {

    auto FinalizeRtoRPlaneWavesExecutionConfigV2(FRtoRPlaneWavesExecutionConfig &cfg) -> void {
        if (cfg.N == 0) throw Exception("RtoR requires N > 0.");

        if (cfg.Dt <= 0.0) {
            const auto h = cfg.L / static_cast<DevFloat>(cfg.N);
            cfg.Dt = static_cast<DevFloat>(0.1) * h; // Legacy KG default: dt/h = 0.1
        }
    }

    auto BuildRtoRPlaneWavesRecipeConfigV2(const FRtoRPlaneWavesExecutionConfig &cfg)
        -> Slab::Models::KGRtoR::PlaneWaves::V2::FKGRtoRPlaneWavesConfigV2 {
        Slab::Models::KGRtoR::PlaneWaves::V2::FKGRtoRPlaneWavesConfigV2 recipeCfg;
        recipeCfg.N = cfg.N;
        recipeCfg.L = cfg.L;
        recipeCfg.Dt = cfg.Dt;
        recipeCfg.Steps = cfg.Steps;
        recipeCfg.XCenter = cfg.XCenter;
        recipeCfg.Q = cfg.Q;
        recipeCfg.Harmonic = cfg.Harmonic;
        return recipeCfg;
    }

    auto BuildRtoRPlaneWavesRecipeV2(const FRtoRPlaneWavesExecutionConfig &cfg,
                                     const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2> {
        using namespace Slab::Models::KGRtoR::PlaneWaves::V2;

        const auto recipeCfg = BuildRtoRPlaneWavesRecipeConfigV2(cfg);
        if (liveView != nullptr) {
            auto recipe = New<FKGRtoRPlaneWavesRecipeV2>(recipeCfg, cfg.Interval, liveView);
            recipe->SetLiveViewIntervalSteps(cfg.MonitorInterval);
            return recipe;
        }

        return New<FKGRtoRPlaneWavesRecipeV2>(recipeCfg, cfg.Interval);
    }

    auto BuildRtoRPlaneWavesPassiveMonitorWindowV2(
        const FRtoRPlaneWavesExecutionConfig &cfg,
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView) -> TPointer<Graphics::FSlabWindow> {
        if (liveView == nullptr) throw Exception("RtoR passive monitor requires a live view.");
        return New<Slab::Studios::Common::Monitors::V2::FRtoRPlaneWavesPassiveMonitorWindowV2>(liveView, cfg.Steps);
    }

    auto RunRtoRPlaneWavesV2(const FRtoRPlaneWavesExecutionConfig &cfg) -> int {
        using namespace Slab::Math::Numerics::V2;

        auto runCfg = cfg;
        FinalizeRtoRPlaneWavesExecutionConfigV2(runCfg);

        if (runCfg.bEnableGLMonitor) {
            auto liveView = New<Math::LiveData::V2::FSessionLiveViewV2>();
            auto recipe = BuildRtoRPlaneWavesRecipeV2(runCfg, liveView);
            auto monitor = BuildRtoRPlaneWavesPassiveMonitorWindowV2(runCfg, liveView);
            return Slab::Studios::Common::RunGLFWMonitoredNumericTaskV2(
                "Studios KGRtoR Plane Waves V2 Monitor", recipe, monitor, static_cast<size_t>(runCfg.Batch));
        }

        auto recipe = BuildRtoRPlaneWavesRecipeV2(runCfg);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(runCfg.Batch));
        const auto status = Slab::Studios::Common::RunTaskAndWait(*task);
        Slab::Studios::Common::PrintNumericTaskSummary(*task);
        return Slab::Studios::Common::ExitCodeFromTaskStatus(status);
    }

} // namespace Slab::Studios::Common::Simulations::V2
