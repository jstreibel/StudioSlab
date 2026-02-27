#include "MolecularDynamics-Baseline-RecipeV2.h"

#include "Core/Controller/Parameter/BuiltinParameters.h"
#include "Core/SlabCore.h"

#include "Math/Numerics/V2/Listeners/ConsoleProgressListenerV2.h"
#include "Math/Numerics/V2/Listeners/SessionLiveViewPublisherListenerV2.h"
#include "Math/Numerics/V2/Runtime/StepperSessionV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"

#include "Models/MolecularDynamics/VerletStepper.cpp"
#include "Models/MolecularDynamics/VerletStepper.h"
#include "Models/MolecularDynamics/Hamiltonians/SoftDisk/SoftDisk.h"
#include "Models/MolecularDynamics/Hamiltonians/Lennard-Jones/LennardJones.h"

#include <algorithm>
#include <limits>

namespace Slab::Models::MolecularDynamics::V2 {

    using namespace Slab::Math::Numerics::V2;

    FMolecularDynamicsBaselineRecipeV2::FMolecularDynamicsBaselineRecipeV2(
            FMolecularDynamicsBaselineConfigV2 config,
            const UIntBig consoleIntervalSteps,
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
    : Config(std::move(config))
    , ConsoleIntervalSteps(std::max<UIntBig>(1, consoleIntervalSteps))
    , LiveView(liveView) {
        ValidateConfig();
    }

    auto FMolecularDynamicsBaselineRecipeV2::ValidateConfig() const -> void {
        if (Config.N < 128) throw Exception("MolecularDynamics V2 requires N >= 128.");
        if (Config.L <= 0.0) throw Exception("MolecularDynamics V2 requires L > 0.");
        if (Config.TotalTime <= 0.0) throw Exception("MolecularDynamics V2 requires total time > 0.");
        if (Config.Steps == 0) throw Exception("MolecularDynamics V2 requires steps > 0.");
        if (Config.Steps > static_cast<UIntBig>(std::numeric_limits<int>::max())) {
            throw Exception("MolecularDynamics V2 steps exceed supported int range.");
        }
        if (Config.N > static_cast<UInt>(std::numeric_limits<int>::max())) {
            throw Exception("MolecularDynamics V2 N exceeds supported int range.");
        }
    }

    auto FMolecularDynamicsBaselineRecipeV2::BuildNumericConfig() const -> TPointer<MolDynNumericConfig> {
        ValidateConfig();

        auto numericConfig = New<MolDynNumericConfig>(false);
        const auto iface = numericConfig->GetInterface();
        if (iface == nullptr) throw Exception("MolecularDynamics V2 numeric config interface is null.");

        auto pN = DynamicPointerCast<Core::IntegerParameter>(iface->GetParameter("N"));
        auto pL = DynamicPointerCast<Core::RealParameter>(iface->GetParameter("L"));
        auto pT = DynamicPointerCast<Core::RealParameter>(iface->GetParameter("t"));
        auto pSteps = DynamicPointerCast<Core::IntegerParameter>(iface->GetParameter("steps"));

        if (pN == nullptr || pL == nullptr || pT == nullptr || pSteps == nullptr) {
            throw Exception("MolecularDynamics V2 numeric config is missing expected parameters.");
        }

        pN->SetValue(static_cast<int>(Config.N));
        pL->SetValue(Config.L);
        pT->SetValue(Config.TotalTime);
        pSteps->SetValue(static_cast<int>(Config.Steps));

        return numericConfig;
    }

    auto FMolecularDynamicsBaselineRecipeV2::BuildStepper(
            const TPointer<MolDynNumericConfig> &numericConfig) const -> TPointer<Math::FStepper> {
        if (numericConfig == nullptr) throw Exception("MolecularDynamics V2 stepper requires numeric config.");

        if (Config.InteractionModel == EInteractionModelV2::LennardJones) {
            LennardJones mechanics(numericConfig, Config.Temperature);
            mechanics.setTemperature(Config.Temperature);
            mechanics.setDissipation(Config.Dissipation);
            return New<TVerletStepper<LennardJones>>(numericConfig, mechanics);
        }

        SoftDisk mechanics(numericConfig, Config.Temperature);
        mechanics.setTemperature(Config.Temperature);
        mechanics.setDissipation(Config.Dissipation);
        return New<TVerletStepper<SoftDisk>>(numericConfig, mechanics);
    }

    auto FMolecularDynamicsBaselineRecipeV2::BuildSession() -> TPointer<FSimulationSessionV2> {
        auto numericConfig = BuildNumericConfig();
        const auto dt = numericConfig->Get_dt();
        auto stepper = BuildStepper(numericConfig);

        return New<FStepperSessionV2>(stepper, dt, 0, DevFloat(0.0));
    }

    auto FMolecularDynamicsBaselineRecipeV2::BuildDefaultSubscriptions() -> Vector<FSubscriptionV2> {
        const auto liveViewInterval = LiveViewIntervalSteps > 0 ? LiveViewIntervalSteps : ConsoleIntervalSteps;

        auto console = New<FConsoleProgressListenerV2>(Config.Steps, "MolecularDynamics Console V2");
        Vector<FSubscriptionV2> subscriptions = {{
            New<FEveryNStepsTriggerV2>(ConsoleIntervalSteps),
            console,
            EDeliveryModeV2::Synchronous,
            true,
            true
        }};

        if (LiveView != nullptr) {
            auto publisher = New<FSessionLiveViewPublisherListenerV2>(
                LiveView,
                "MolecularDynamics Live View Publisher V2");
            subscriptions.push_back({
                New<FEveryNStepsTriggerV2>(liveViewInterval),
                publisher,
                EDeliveryModeV2::Synchronous,
                true,
                true
            });
        }

        return subscriptions;
    }

    auto FMolecularDynamicsBaselineRecipeV2::SetLiveViewIntervalSteps(const UIntBig liveViewIntervalSteps) -> void {
        LiveViewIntervalSteps = liveViewIntervalSteps == 0
            ? UIntBig(0)
            : std::max<UIntBig>(1, liveViewIntervalSteps);
    }

    auto FMolecularDynamicsBaselineRecipeV2::GetRunLimits() const -> FRunLimitsV2 {
        ValidateConfig();

        FRunLimitsV2 limits;
        limits.Mode = ERunModeV2::FiniteSteps;
        limits.MaxSteps = Config.Steps;
        return limits;
    }

    auto FMolecularDynamicsBaselineRecipeV2::GetConfig() const -> const FMolecularDynamicsBaselineConfigV2 & {
        return Config;
    }

    auto FMolecularDynamicsBaselineRecipeV2::GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2> {
        return LiveView;
    }

} // namespace Slab::Models::MolecularDynamics::V2
