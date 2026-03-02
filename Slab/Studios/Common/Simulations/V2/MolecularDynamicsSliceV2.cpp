#include "MolecularDynamicsSliceV2.h"

#include "../../Monitors/V2/MolecularDynamicsPassiveMonitorWindowV2.h"
#include "../../NumericsV2TaskUtils.h"
#include "../../V2SimulationRunners.h"

#include "Math/Numerics/V2/Listeners/StateSnapshotListenerV2.h"
#include "Math/Numerics/V2/Runtime/AppendedSubscriptionsRecipeV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/MolecularDynamics/V2/MolecularDynamics-Baseline-RecipeV2.h"

#include <algorithm>

namespace Slab::Studios::Common::Simulations::V2 {

    namespace {

        auto ToModelConfig(const FMolecularDynamicsExecutionConfigV2 &cfg)
            -> Slab::Models::MolecularDynamics::V2::FMolecularDynamicsBaselineConfigV2 {
            using namespace Slab::Models::MolecularDynamics::V2;

            FMolecularDynamicsBaselineConfigV2 modelCfg;
            modelCfg.N = cfg.N;
            modelCfg.L = cfg.L;
            modelCfg.TotalTime = cfg.TotalTime;
            modelCfg.Steps = cfg.Steps;
            modelCfg.Temperature = cfg.Temperature;
            modelCfg.Dissipation = cfg.Dissipation;
            modelCfg.InteractionModel = cfg.InteractionModel == EMDInteractionModelV2::LennardJones
                ? EInteractionModelV2::LennardJones
                : EInteractionModelV2::SoftDisk;
            return modelCfg;
        }

    } // namespace

    auto FinalizeMolecularDynamicsExecutionConfigV2(FMolecularDynamicsExecutionConfigV2 &cfg) -> void {
        if (cfg.N < 128) throw Exception("MolecularDynamics V2 requires N >= 128.");
        if (cfg.L <= 0.0) throw Exception("MolecularDynamics V2 requires L > 0.");
        if (cfg.TotalTime <= 0.0) throw Exception("MolecularDynamics V2 requires total time > 0.");
        if (cfg.Steps == 0) throw Exception("MolecularDynamics V2 requires steps > 0.");
        if (cfg.Interval == 0) cfg.Interval = 1;
        if (cfg.MonitorInterval == 0) cfg.MonitorInterval = cfg.Interval;
    }

    auto BuildMolecularDynamicsRecipeV2(
        const FMolecularDynamicsExecutionConfigV2 &cfg,
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2> {
        using namespace Slab::Models::MolecularDynamics::V2;

        const auto modelCfg = ToModelConfig(cfg);
        auto recipe = New<FMolecularDynamicsBaselineRecipeV2>(
            modelCfg,
            std::max<UIntBig>(UIntBig(1), cfg.Interval),
            liveView);
        recipe->SetLiveViewIntervalSteps(cfg.MonitorInterval);
        return recipe;
    }

    auto RunMolecularDynamicsV2(const FMolecularDynamicsExecutionConfigV2 &cfg) -> int {
        using namespace Slab::Math::Numerics::V2;

        auto runCfg = cfg;
        FinalizeMolecularDynamicsExecutionConfigV2(runCfg);

        if (runCfg.bEnableGLMonitor) {
            auto liveView = New<Math::LiveData::V2::FSessionLiveViewV2>();
            auto baseRecipe = BuildMolecularDynamicsRecipeV2(runCfg, liveView);
            auto snapshotListener = New<FStateSnapshotListenerV2>("MolecularDynamics monitor snapshot listener V2");

            Vector<FSubscriptionV2> subscriptions = {{
                New<FEveryNStepsTriggerV2>(std::max<UIntBig>(UIntBig(1), runCfg.MonitorInterval)),
                snapshotListener,
                EDeliveryModeV2::LatestOnly,
                true,
                true
            }};

            auto recipe = New<FAppendedSubscriptionsRecipeV2>(baseRecipe, std::move(subscriptions));
            auto monitor = BuildMolecularDynamicsPassiveMonitorWindowV2(runCfg, liveView, snapshotListener);
            return Slab::Studios::Common::RunGLFWMonitoredNumericTaskV2(
                "Studios MolecularDynamics V2 Monitor",
                recipe,
                monitor,
                static_cast<size_t>(runCfg.Batch));
        }

        auto recipe = BuildMolecularDynamicsRecipeV2(runCfg);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(runCfg.Batch));
        const auto status = Slab::Studios::Common::RunTaskAndWait(*task);
        Slab::Studios::Common::PrintNumericTaskSummary(*task);
        return Slab::Studios::Common::ExitCodeFromTaskStatus(status);
    }

    auto BuildMolecularDynamicsPassiveMonitorWindowV2(
            const FMolecularDynamicsExecutionConfigV2 &cfg,
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            const TPointer<Math::Numerics::V2::FStateSnapshotListenerV2> &snapshotListener)
        -> TPointer<Graphics::FSlabWindow> {
        if (liveView == nullptr) throw Exception("MolecularDynamics passive monitor requires a live view.");
        if (snapshotListener == nullptr) throw Exception("MolecularDynamics passive monitor requires a snapshot listener.");
        return New<Slab::Studios::Common::Monitors::V2::FMolecularDynamicsPassiveMonitorWindowV2>(
            liveView,
            snapshotListener,
            cfg.Steps,
            cfg.L);
    }

} // namespace Slab::Studios::Common::Simulations::V2
