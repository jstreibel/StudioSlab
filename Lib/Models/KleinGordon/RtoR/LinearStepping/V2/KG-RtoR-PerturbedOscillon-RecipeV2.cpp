#include "KG-RtoR-PerturbedOscillon-RecipeV2.h"

#include "Core/SlabCore.h"

#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/PerturbedOscillonTimeDerivative.h"
#include "Math/Function/RtoR/Model/Operators/RtoRLaplacian.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Math/Numerics/ODE/Steppers/RungeKutta4.h"
#include "Math/Numerics/V2/Listeners/ConsoleProgressListenerV2.h"
#include "Math/Numerics/V2/Listeners/SessionLiveViewPublisherListenerV2.h"
#include "Math/Numerics/V2/Runtime/StepperSessionV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"

#include "../KG-RtoRBoundaryCondition.h"
#include "../KG-RtoREquationState.h"
#include "../KG-RtoRSolver.h"

#include <algorithm>
#include <utility>

namespace Slab::Models::KGRtoR::PerturbedOscillon::V2 {

    using namespace Slab::Math;
    using namespace Slab::Math::Numerics::V2;

    namespace {

        auto MakeFixedField(const FKGRtoRPerturbedOscillonConfigV2 &cfg)
            -> TPointer<RtoR::NumericFunction_CPU> {
            const auto xMin = cfg.XCenter - cfg.L * static_cast<DevFloat>(0.5);
            const auto xMax = cfg.XCenter + cfg.L * static_cast<DevFloat>(0.5);

            return New<RtoR::NumericFunction_CPU>(cfg.N,
                                                  xMin,
                                                  xMax,
                                                  RtoR::NumericFunction::Standard1D_FixedBorder);
        }

    } // namespace

    FKGRtoRPerturbedOscillonRecipeV2::FKGRtoRPerturbedOscillonRecipeV2(
        FKGRtoRPerturbedOscillonConfigV2 config,
        const UIntBig consoleIntervalSteps,
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
        const bool bInRunEndless)
    : Config(std::move(config))
    , ConsoleIntervalSteps(std::max<UIntBig>(1, consoleIntervalSteps))
    , LiveView(liveView)
    , bRunEndless(bInRunEndless) {
        ValidateConfig();
    }

    auto FKGRtoRPerturbedOscillonRecipeV2::ValidateConfig() const -> void {
        if (Config.N < 4) throw Exception("KGRtoR PerturbedOscillon V2 requires N >= 4.");
        if (Config.L <= 0.0) throw Exception("KGRtoR PerturbedOscillon V2 requires L > 0.");
        if (Config.Dt <= 0.0) throw Exception("KGRtoR PerturbedOscillon V2 requires dt > 0.");
        if (!bRunEndless && Config.Steps == 0) {
            throw Exception("KGRtoR PerturbedOscillon V2 requires steps > 0.");
        }
        if (Config.Lambda <= 0.0) {
            throw Exception("KGRtoR PerturbedOscillon V2 requires lambda > 0.");
        }
    }

    auto FKGRtoRPerturbedOscillonRecipeV2::BuildStepperInstance() const
        -> TPointer<::Slab::Math::FStepper> {
        ValidateConfig();

        auto phi = MakeFixedField(Config);
        auto dPhiDt = MakeFixedField(Config);
        auto prototypeState = New<KGRtoR::FEquationState>(phi, dPhiDt);

        auto initialPhi = New<RtoR::NullFunction>();
        auto initialDPhiDt = New<RtoR::PerturbedOscillonTimeDerivative>(Config.Lambda, Config.Epsilon);
        auto boundary = New<KGRtoR::FBoundaryCondition>(prototypeState, initialPhi, initialDPhiDt);
        auto laplacian = New<RtoR::RtoRLaplacian>();
        auto potential = New<RtoR::AbsFunction>();
        auto solver = New<KGRtoR::KGRtoRSolver>(boundary, laplacian, potential);

        return New<FRungeKutta4>(solver, Config.Dt);
    }

    auto FKGRtoRPerturbedOscillonRecipeV2::BuildSession() -> TPointer<FSimulationSessionV2> {
        return New<FStepperSessionV2>(BuildStepperInstance(), Config.Dt, 0, DevFloat(0.0));
    }

    auto FKGRtoRPerturbedOscillonRecipeV2::BuildDefaultSubscriptions() -> Vector<FSubscriptionV2> {
        const auto liveViewInterval = LiveViewIntervalSteps > 0 ? LiveViewIntervalSteps : ConsoleIntervalSteps;

        auto console = New<FConsoleProgressListenerV2>(
            bRunEndless ? std::optional<UIntBig>{} : std::optional<UIntBig>{Config.Steps},
            "KGRtoR Perturbed Oscillon Console V2");
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
                "KGRtoR Perturbed Oscillon Live View Publisher V2");
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

    auto FKGRtoRPerturbedOscillonRecipeV2::SetLiveViewIntervalSteps(const UIntBig liveViewIntervalSteps) -> void {
        LiveViewIntervalSteps = liveViewIntervalSteps == 0 ? UIntBig(0) : std::max<UIntBig>(1, liveViewIntervalSteps);
    }

    auto FKGRtoRPerturbedOscillonRecipeV2::GetRunLimits() const -> FRunLimitsV2 {
        ValidateConfig();

        FRunLimitsV2 limits;
        if (bRunEndless) {
            limits.Mode = ERunModeV2::OpenEnded;
            limits.MaxSteps = std::nullopt;
            return limits;
        }

        limits.Mode = ERunModeV2::FiniteSteps;
        limits.MaxSteps = Config.Steps;
        return limits;
    }

    auto FKGRtoRPerturbedOscillonRecipeV2::GetConfig() const
        -> const FKGRtoRPerturbedOscillonConfigV2 & {
        return Config;
    }

    auto FKGRtoRPerturbedOscillonRecipeV2::GetLiveView() const
        -> TPointer<Math::LiveData::V2::FSessionLiveViewV2> {
        return LiveView;
    }

} // namespace Slab::Models::KGRtoR::PerturbedOscillon::V2
