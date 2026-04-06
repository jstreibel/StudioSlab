#include "MolecularDynamicsSliceV2.h"

#include "../../Monitors/V2/MolecularDynamicsPassiveMonitorWindowV2.h"
#include "../../NumericsV2TaskUtils.h"
#include "../../V2SimulationRunners.h"

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
        if (cfg.N < 128) throw Exception("MolecularDynamics requires N >= 128.");
        if (cfg.L <= 0.0) throw Exception("MolecularDynamics requires L > 0.");
        if (cfg.TotalTime <= 0.0) throw Exception("MolecularDynamics requires total time > 0.");
        if (cfg.Steps == 0) throw Exception("MolecularDynamics requires steps > 0.");
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
            liveView,
            cfg.bRunEndless);
        recipe->SetLiveViewIntervalSteps(cfg.MonitorInterval);
        return recipe;
    }

    auto RunMolecularDynamicsV2(const FMolecularDynamicsExecutionConfigV2 &cfg) -> int {
        using namespace Slab::Math::Numerics::V2;

        auto runCfg = cfg;
        FinalizeMolecularDynamicsExecutionConfigV2(runCfg);

        if (runCfg.bEnableGLMonitor) {
            auto liveView = New<Math::LiveData::V2::FSessionLiveViewV2>();
            auto recipe = BuildMolecularDynamicsRecipeV2(runCfg, liveView);
            auto monitor = BuildMolecularDynamicsPassiveMonitorWindowV2(runCfg, liveView);
            return Slab::Studios::Common::RunGLFWMonitoredNumericTaskV2(
                "Studios MolecularDynamics Monitor",
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
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Graphics::FSlabWindow> {
        if (liveView == nullptr) throw Exception("MolecularDynamics passive monitor requires a live view.");
        return New<Slab::Studios::Common::Monitors::V2::FMolecularDynamicsPassiveMonitorWindowV2>(
            liveView,
            cfg.bRunEndless ? UIntBig(0) : cfg.Steps,
            cfg.L);
    }

} // namespace Slab::Studios::Common::Simulations::V2
