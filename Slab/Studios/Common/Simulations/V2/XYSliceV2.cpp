#include "XYSliceV2.h"

#include "../../Monitors/V2/XYMetropolisPassiveMonitorWindowV2.h"
#include "../../NumericsV2TaskUtils.h"
#include "../../V2SimulationRunners.h"

#include "Math/Numerics/V2/Listeners/StateSnapshotListenerV2.h"
#include "Math/Numerics/V2/Runtime/AppendedSubscriptionsRecipeV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/XY/V2/XY-Metropolis-RecipeV2.h"

#include <algorithm>

namespace Slab::Studios::Common::Simulations::V2 {

    auto FinalizeXYExecutionConfigV2(FXYExecutionConfigV2 &cfg) -> void {
        if (cfg.L < 2) throw Exception("XY V2 requires L >= 2.");
        if (cfg.Steps == 0) throw Exception("XY V2 requires steps > 0.");
        if (cfg.Temperature < 0.0) throw Exception("XY V2 requires temperature >= 0.");
        if (cfg.DeltaTheta <= 0.0) throw Exception("XY V2 requires delta-theta > 0.");
        if (cfg.Interval == 0) cfg.Interval = 1;
        if (cfg.MonitorInterval == 0) cfg.MonitorInterval = cfg.Interval;
    }

    auto BuildXYRecipeV2(const FXYExecutionConfigV2 &cfg,
                         const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2> {
        using namespace Slab::Models::XY::V2;

        FXYMetropolisConfigV2 modelCfg;
        modelCfg.L = cfg.L;
        modelCfg.Steps = cfg.Steps;
        modelCfg.Temperature = cfg.Temperature;
        modelCfg.ExternalField = cfg.ExternalField;
        modelCfg.DeltaTheta = cfg.DeltaTheta;
        modelCfg.bFerromagneticInitial = cfg.bFerromagneticInitial;

        auto recipe = New<FXYMetropolisRecipeV2>(
            modelCfg,
            std::max<UIntBig>(UIntBig(1), cfg.Interval),
            liveView);
        recipe->SetLiveViewIntervalSteps(cfg.MonitorInterval);
        return recipe;
    }

    auto BuildXYPassiveMonitorWindowV2(const FXYExecutionConfigV2 &cfg,
                                       const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
                                       const TPointer<Math::Numerics::V2::FStateSnapshotListenerV2> &snapshotListener)
        -> TPointer<Graphics::FSlabWindow> {
        (void) cfg;
        if (liveView == nullptr) throw Exception("XY passive monitor requires a live view.");
        if (snapshotListener == nullptr) throw Exception("XY passive monitor requires a snapshot listener.");
        return New<Slab::Studios::Common::Monitors::V2::FXYMetropolisPassiveMonitorWindowV2>(
            liveView,
            snapshotListener,
            cfg.Steps);
    }

    auto RunXYV2(const FXYExecutionConfigV2 &cfg) -> int {
        using namespace Slab::Math::Numerics::V2;

        auto runCfg = cfg;
        FinalizeXYExecutionConfigV2(runCfg);

        if (runCfg.bEnableGLMonitor) {
            auto liveView = New<Math::LiveData::V2::FSessionLiveViewV2>();
            auto baseRecipe = BuildXYRecipeV2(runCfg, liveView);
            auto snapshotListener = New<FStateSnapshotListenerV2>("XY monitor snapshot listener V2");

            Vector<FSubscriptionV2> subscriptions = {{
                New<FEveryNStepsTriggerV2>(std::max<UIntBig>(UIntBig(1), runCfg.MonitorInterval)),
                snapshotListener,
                EDeliveryModeV2::LatestOnly,
                true,
                true
            }};

            auto recipe = New<FAppendedSubscriptionsRecipeV2>(baseRecipe, std::move(subscriptions));
            auto monitor = BuildXYPassiveMonitorWindowV2(runCfg, liveView, snapshotListener);
            return Slab::Studios::Common::RunGLFWMonitoredNumericTaskV2(
                "Studios XY V2 Monitor",
                recipe,
                monitor,
                static_cast<size_t>(runCfg.Batch));
        }

        auto recipe = BuildXYRecipeV2(runCfg);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(runCfg.Batch));
        const auto status = Slab::Studios::Common::RunTaskAndWait(*task);
        Slab::Studios::Common::PrintNumericTaskSummary(*task);
        return Slab::Studios::Common::ExitCodeFromTaskStatus(status);
    }

} // namespace Slab::Studios::Common::Simulations::V2
