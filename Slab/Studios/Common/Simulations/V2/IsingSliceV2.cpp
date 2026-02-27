#include "IsingSliceV2.h"

#include "../../Monitors/V2/IsingMetropolisPassiveMonitorWindowV2.h"
#include "../../NumericsV2TaskUtils.h"
#include "../../V2SimulationRunners.h"

#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/Ising/V2/Ising-Metropolis-RecipeV2.h"

#include <algorithm>

namespace Slab::Studios::Common::Simulations::V2 {

    auto FinalizeIsingExecutionConfigV2(FIsingExecutionConfigV2 &cfg) -> void {
        if (cfg.L < 2) throw Exception("Ising V2 requires L >= 2.");
        if (cfg.Steps == 0) throw Exception("Ising V2 requires steps > 0.");
        if (cfg.Temperature < 0.0) throw Exception("Ising V2 requires temperature >= 0.");
        if (cfg.Interval == 0) cfg.Interval = 1;
        if (cfg.MonitorInterval == 0) cfg.MonitorInterval = cfg.Interval;
    }

    auto BuildIsingRecipeV2(const FIsingExecutionConfigV2 &cfg,
                            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2> {
        using namespace Slab::Models::Ising::V2;

        FIsingMetropolisConfigV2 modelCfg;
        modelCfg.L = cfg.L;
        modelCfg.Steps = cfg.Steps;
        modelCfg.Temperature = cfg.Temperature;
        modelCfg.ExternalField = cfg.ExternalField;
        modelCfg.bFerromagneticInitial = cfg.bFerromagneticInitial;

        auto recipe = New<FIsingMetropolisRecipeV2>(
            modelCfg,
            std::max<UIntBig>(UIntBig(1), cfg.Interval),
            liveView);
        recipe->SetLiveViewIntervalSteps(cfg.MonitorInterval);
        return recipe;
    }

    auto BuildIsingPassiveMonitorWindowV2(const FIsingExecutionConfigV2 &cfg,
                                          const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Graphics::FSlabWindow> {
        (void) cfg;
        if (liveView == nullptr) throw Exception("Ising passive monitor requires a live view.");
        return New<Slab::Studios::Common::Monitors::V2::FIsingMetropolisPassiveMonitorWindowV2>(liveView, cfg.Steps);
    }

    auto RunIsingV2(const FIsingExecutionConfigV2 &cfg) -> int {
        using namespace Slab::Math::Numerics::V2;

        auto runCfg = cfg;
        FinalizeIsingExecutionConfigV2(runCfg);

        if (runCfg.bEnableGLMonitor) {
            auto liveView = New<Math::LiveData::V2::FSessionLiveViewV2>();
            auto recipe = BuildIsingRecipeV2(runCfg, liveView);
            auto monitor = BuildIsingPassiveMonitorWindowV2(runCfg, liveView);
            return Slab::Studios::Common::RunGLFWMonitoredNumericTaskV2(
                "Studios Ising V2 Monitor",
                recipe,
                monitor,
                static_cast<size_t>(runCfg.Batch));
        }

        auto recipe = BuildIsingRecipeV2(runCfg);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(runCfg.Batch));
        const auto status = Slab::Studios::Common::RunTaskAndWait(*task);
        Slab::Studios::Common::PrintNumericTaskSummary(*task);
        return Slab::Studios::Common::ExitCodeFromTaskStatus(status);
    }

} // namespace Slab::Studios::Common::Simulations::V2
