#include "KGR2toRBaselineSliceV2.h"

#include "../../Monitors/V2/KGR2toRBaselinePassiveMonitorWindowV2.h"
#include "../../NumericsV2TaskUtils.h"
#include "../../V2SimulationRunners.h"

#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/KleinGordon/R2toR/V2/KG-R2toR-Baseline-RecipeV2.h"

#include <algorithm>

namespace Slab::Studios::Common::Simulations::V2 {

    auto FinalizeR2toRBaselineExecutionConfigV2(FR2toRBaselineExecutionConfig &cfg) -> void {
        if (cfg.N == 0) throw Exception("KGR2toR baseline requires N > 0.");
        if (cfg.Interval == 0) cfg.Interval = 1;
        if (cfg.MonitorInterval == 0) cfg.MonitorInterval = cfg.Interval;
    }

    auto BuildR2toRBaselineRecipeConfigV2(const FR2toRBaselineExecutionConfig &cfg)
        -> Slab::Models::KGR2toR::Baseline::V2::FKGR2toRBaselineConfigV2 {
        Slab::Models::KGR2toR::Baseline::V2::FKGR2toRBaselineConfigV2 recipeCfg;
        recipeCfg.N = cfg.N;
        recipeCfg.L = cfg.L;
        recipeCfg.RDt = cfg.RDt;
        recipeCfg.Steps = cfg.Steps;
        recipeCfg.XCenter = cfg.XCenter;
        recipeCfg.YCenter = cfg.YCenter;
        recipeCfg.PulseWidth = cfg.PulseWidth;
        recipeCfg.PhiAmplitude = cfg.PhiAmplitude;
        recipeCfg.DPhiDtAmplitude = cfg.DPhiDtAmplitude;
        return recipeCfg;
    }

    auto BuildR2toRBaselineRecipeV2(const FR2toRBaselineExecutionConfig &cfg,
                                    const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2> {
        using namespace Slab::Models::KGR2toR::Baseline::V2;

        const auto recipeCfg = BuildR2toRBaselineRecipeConfigV2(cfg);
        auto recipe = New<FKGR2toRBaselineRecipeV2>(recipeCfg, std::max<UIntBig>(UIntBig(1), cfg.Interval), liveView);
        recipe->SetLiveViewIntervalSteps(cfg.MonitorInterval);
        return recipe;
    }

    auto BuildR2toRBaselinePassiveMonitorWindowV2(
            const FR2toRBaselineExecutionConfig &cfg,
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView) -> TPointer<Graphics::FSlabWindow> {
        if (liveView == nullptr) throw Exception("KGR2toR passive monitor requires a live view.");
        return New<Slab::Studios::Common::Monitors::V2::FR2toRBaselinePassiveMonitorWindowV2>(liveView, cfg.Steps);
    }

    auto RunR2toRBaselineV2(const FR2toRBaselineExecutionConfig &cfg) -> int {
        using namespace Slab::Math::Numerics::V2;

        auto runCfg = cfg;
        FinalizeR2toRBaselineExecutionConfigV2(runCfg);

        if (runCfg.bEnableGLMonitor) {
            auto liveView = New<Math::LiveData::V2::FSessionLiveViewV2>();
            auto recipe = BuildR2toRBaselineRecipeV2(runCfg, liveView);
            auto monitor = BuildR2toRBaselinePassiveMonitorWindowV2(runCfg, liveView);
            return Slab::Studios::Common::RunGLFWMonitoredNumericTaskV2(
                "Studios KGR2toR Baseline V2 Monitor",
                recipe,
                monitor,
                static_cast<size_t>(runCfg.Batch));
        }

        auto recipe = BuildR2toRBaselineRecipeV2(runCfg);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(runCfg.Batch));
        const auto status = Slab::Studios::Common::RunTaskAndWait(*task);
        Slab::Studios::Common::PrintNumericTaskSummary(*task);
        return Slab::Studios::Common::ExitCodeFromTaskStatus(status);
    }

} // namespace Slab::Studios::Common::Simulations::V2
