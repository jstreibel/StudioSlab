#include "IsingSliceV2.h"

#include "../../Monitors/V2/IsingMetropolisPassiveMonitorWindowV2.h"
#include "../../NumericsV2TaskUtils.h"
#include "../../V2SimulationRunners.h"

#include "LiveParameterControlV2.h"
#include "Math/Numerics/V2/Runtime/AppendedSubscriptionsRecipeV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/Ising/V2/Ising-Metropolis-RecipeV2.h"

#include <algorithm>
#include <utility>

namespace Slab::Studios::Common::Simulations::V2 {

    auto FinalizeIsingExecutionConfigV2(FIsingExecutionConfigV2 &cfg) -> void {
        if (cfg.L < 2) throw Exception("Ising requires L >= 2.");
        if (cfg.Steps == 0) throw Exception("Ising requires steps > 0.");
        if (cfg.Temperature < 0.0) throw Exception("Ising requires temperature >= 0.");
        if (cfg.Interval == 0) cfg.Interval = 1;
        if (cfg.MonitorInterval == 0) cfg.MonitorInterval = cfg.Interval;

        if (cfg.bEnableLiveParameterBinding) {
            if (cfg.ControlSampleInterval == 0) cfg.ControlSampleInterval = 1;
            if (cfg.ControlTopicPrefix.empty()) throw Exception("Ising live parameter binding requires a non-empty topic prefix.");
            if (cfg.ControlHub == nullptr) cfg.ControlHub = New<Math::LiveControl::V2::FLiveControlHubV2>();
        }
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

        auto baseRecipe = New<FIsingMetropolisRecipeV2>(
            modelCfg,
            std::max<UIntBig>(UIntBig(1), cfg.Interval),
            liveView);
        baseRecipe->SetLiveViewIntervalSteps(cfg.MonitorInterval);

        if (!cfg.bEnableLiveParameterBinding || cfg.ControlHub == nullptr) return baseRecipe;

        const auto controls = baseRecipe->GetRuntimeControls();
        if (controls == nullptr) return baseRecipe;

        using EMutability = ELiveParameterMutabilityV2;
        using EExposure = ELiveParameterExposureV2;

        Vector<FLiveScalarParameterBindingV2> bindings;
        bindings.push_back({
            "temperature",
            "Temperature",
            "Monte Carlo temperature (runtime mutable).",
            BuildLiveScalarParameterTopicV2(cfg.ControlTopicPrefix, "temperature"),
            0.0,
            std::nullopt,
            EMutability::RuntimeMutable,
            EExposure::WritableExposed,
            cfg.Temperature,
            [controls]() { return controls->GetTemperature(); },
            [controls](const DevFloat value) { controls->SetTemperature(value); }
        });
        bindings.push_back({
            "external_field",
            "h-field",
            "External magnetic field (runtime mutable).",
            BuildLiveScalarParameterTopicV2(cfg.ControlTopicPrefix, "external_field"),
            std::nullopt,
            std::nullopt,
            EMutability::RuntimeMutable,
            EExposure::WritableExposed,
            cfg.ExternalField,
            [controls]() { return controls->GetExternalField(); },
            [controls](const DevFloat value) { controls->SetExternalField(value); }
        });
        bindings.push_back({
            "L",
            "L",
            "Lattice side length (read-only at runtime).",
            BuildLiveScalarParameterTopicV2(cfg.ControlTopicPrefix, "L"),
            std::nullopt,
            std::nullopt,
            EMutability::Const,
            EExposure::ReadOnlyExposed,
            static_cast<DevFloat>(cfg.L)
        });
        bindings.push_back({
            "steps",
            "Steps",
            "Planned number of sweeps (read-only at runtime).",
            BuildLiveScalarParameterTopicV2(cfg.ControlTopicPrefix, "steps"),
            1.0,
            std::nullopt,
            EMutability::Const,
            EExposure::ReadOnlyExposed,
            static_cast<DevFloat>(cfg.Steps)
        });

        PublishLiveScalarBindingsV2(cfg.ControlHub, bindings);
        const auto bindingSubscriptions = BuildLiveScalarBindingSubscriptionsV2(
            cfg.ControlHub,
            bindings,
            cfg.ControlSampleInterval,
            "Ising live parameter binding");
        if (bindingSubscriptions.empty()) return baseRecipe;

        return New<Math::Numerics::V2::FAppendedSubscriptionsRecipeV2>(baseRecipe, std::move(bindingSubscriptions));
    }

    auto BuildIsingPassiveMonitorWindowV2(const FIsingExecutionConfigV2 &cfg,
                                          const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Graphics::FSlabWindow> {
        (void) cfg;
        if (liveView == nullptr) throw Exception("Ising passive monitor requires a live view.");
        return New<Slab::Studios::Common::Monitors::V2::FIsingMetropolisPassiveMonitorWindowV2>(
            liveView,
            cfg.Steps);
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
                "Studios Ising Monitor",
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
