#include "MetropolisSliceV2.h"

#include "../../Monitors/V2/MetropolisFieldSnapshotListenerV2.h"
#include "../../Monitors/V2/MetropolisPassiveMonitorWindowV2.h"
#include "../../NumericsV2TaskUtils.h"
#include "../../V2SimulationRunners.h"

#include "Math/Numerics/V2/Runtime/AppendedSubscriptionsRecipeV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/KleinGordon/RtoR-Montecarlo/V2/RtoR-Hamiltonian-MetropolisHastings-RecipeV2.h"

#include <algorithm>

namespace Slab::Studios::Common::Simulations::V2 {

    auto FinalizeMetropolisExecutionConfigV2(FMetropolisExecutionConfigV2 &cfg) -> void {
        if (cfg.Steps == 0) throw Exception("Metropolis V2 requires steps > 0.");
        if (cfg.Interval == 0) cfg.Interval = 1;
        if (cfg.MonitorInterval == 0) cfg.MonitorInterval = cfg.Interval;
    }

    auto BuildMetropolisRecipeV2(const FMetropolisExecutionConfigV2 &cfg)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2> {
        using namespace Slab::Models::KGRtoR::Metropolis::V2;

        return New<FRtoRHamiltonianMetropolisHastingsRecipeV2>(
            cfg.Steps,
            std::max<UIntBig>(UIntBig(1), cfg.Interval));
    }

    auto BuildMetropolisMonitorBundleV2(const FMetropolisExecutionConfigV2 &cfg) -> FMetropolisMonitorBundleV2 {
        using namespace Slab::Math::Numerics::V2;
        using namespace Slab::Models::KGRtoR::Metropolis::V2;
        using namespace Slab::Studios::Common::Monitors::V2;

        auto baseRecipe = New<FRtoRHamiltonianMetropolisHastingsRecipeV2>(
            cfg.Steps,
            std::max<UIntBig>(UIntBig(1), cfg.Interval));
        auto snapshotListener = New<FMetropolisFieldSnapshotListenerV2>(baseRecipe);

        Vector<FSubscriptionV2> subscriptions = {{
            New<FEveryNStepsTriggerV2>(std::max<UIntBig>(UIntBig(1), cfg.MonitorInterval)),
            snapshotListener,
            EDeliveryModeV2::Synchronous,
            true,
            true
        }};

        FMetropolisMonitorBundleV2 bundle;
        bundle.Recipe = New<FAppendedSubscriptionsRecipeV2>(baseRecipe, std::move(subscriptions));
        bundle.MonitorWindow = New<FMetropolisPassiveMonitorWindowV2>(snapshotListener, cfg.Steps);
        return bundle;
    }

    auto RunMetropolisV2(const FMetropolisExecutionConfigV2 &cfg) -> int {
        using namespace Slab::Math::Numerics::V2;

        auto runCfg = cfg;
        FinalizeMetropolisExecutionConfigV2(runCfg);

        if (runCfg.bEnableGLMonitor) {
            const auto bundle = BuildMetropolisMonitorBundleV2(runCfg);
            return Slab::Studios::Common::RunGLFWMonitoredNumericTaskV2(
                "Studios Metropolis V2 Monitor",
                bundle.Recipe,
                bundle.MonitorWindow,
                static_cast<size_t>(runCfg.Batch));
        }

        auto recipe = BuildMetropolisRecipeV2(runCfg);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(runCfg.Batch));
        const auto status = Slab::Studios::Common::RunTaskAndWait(*task);
        Slab::Studios::Common::PrintNumericTaskSummary(*task);
        return Slab::Studios::Common::ExitCodeFromTaskStatus(status);
    }

} // namespace Slab::Studios::Common::Simulations::V2
