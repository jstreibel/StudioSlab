#include "SPI-RecipeV2.h"

#include "Core/SlabCore.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Numerics/ODE/Steppers/Euler.h"
#include "Math/Numerics/V2/Listeners/ConsoleProgressListenerV2.h"
#include "Math/Numerics/V2/Listeners/SessionLiveViewPublisherListenerV2.h"
#include "Math/Numerics/V2/Runtime/StepperSessionV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"

#include "../SPI-BC.h"
#include "../SPI-Solver.h"
#include "../SPI-State.h"

#include <algorithm>
#include <cmath>

namespace Slab::Models::StochasticPathIntegrals::V2 {

    using namespace Slab::Math;
    using namespace Slab::Math::Numerics::V2;

    FSPIRecipeV2::FSPIRecipeV2(const TPointer<SPINumericConfig> &numericConfig,
                               const UIntBig consoleIntervalSteps,
                               const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
    : NumericConfig(numericConfig != nullptr ? numericConfig : New<SPINumericConfig>())
    , ConsoleIntervalSteps(std::max<UIntBig>(1, consoleIntervalSteps))
    , LiveView(liveView) {
    }

    auto FSPIRecipeV2::BuildStepper() const -> TPointer<FStepper> {
        if (NumericConfig == nullptr) throw Exception("FSPIRecipeV2 requires a numeric config.");

        const auto L = NumericConfig->getL();
        const auto t = NumericConfig->gett();
        const auto N = NumericConfig->getN();
        const auto dT = NumericConfig->getdT();

        const auto xMin = -L / 2;
        const auto h = L / static_cast<DevFloat>(N);
        constexpr DevFloat tMin = 0.0;
        const auto M = static_cast<Int>(std::floor(t / h));

        auto phi = New<R2toR::NumericFunction_CPU>(N, M, xMin, tMin, h, h);
        auto prototypeState = New<SPIState>(phi);

        auto bc = New<SPIBC>(prototypeState);
        auto solver = New<SPISolver>(bc);

        return New<FEuler>(solver, dT);
    }

    auto FSPIRecipeV2::BuildSession() -> TPointer<FSimulationSessionV2> {
        if (NumericConfig == nullptr) throw Exception("FSPIRecipeV2 requires a numeric config.");

        return New<FStepperSessionV2>(BuildStepper(),
                                      NumericConfig->getdT(),
                                      0,
                                      DevFloat(0.0));
    }

    auto FSPIRecipeV2::BuildDefaultSubscriptions() -> Vector<FSubscriptionV2> {
        const auto totalSteps = NumericConfig != nullptr ? NumericConfig->Get_n() : UIntBig(0);
        const auto liveViewInterval = LiveViewIntervalSteps > 0 ? LiveViewIntervalSteps : ConsoleIntervalSteps;
        auto console = New<FConsoleProgressListenerV2>(totalSteps, "SPI Console V2");
        Vector<FSubscriptionV2> subscriptions = {{
            New<FEveryNStepsTriggerV2>(ConsoleIntervalSteps),
            console,
            EDeliveryModeV2::Synchronous,
            true,
            true
        }};

        if (LiveView != nullptr) {
            auto publisher = New<FSessionLiveViewPublisherListenerV2>(LiveView, "SPI Live View Publisher V2");
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

    auto FSPIRecipeV2::SetLiveViewIntervalSteps(const UIntBig liveViewIntervalSteps) -> void {
        LiveViewIntervalSteps = liveViewIntervalSteps == 0 ? UIntBig(0) : std::max<UIntBig>(1, liveViewIntervalSteps);
    }

    auto FSPIRecipeV2::GetRunLimits() const -> FRunLimitsV2 {
        if (NumericConfig == nullptr) throw Exception("FSPIRecipeV2 requires a numeric config.");

        FRunLimitsV2 limits;
        limits.Mode = ERunModeV2::FiniteSteps;
        limits.MaxSteps = NumericConfig->Get_n();
        return limits;
    }

    auto FSPIRecipeV2::GetNumericConfig() const -> TPointer<SPINumericConfig> {
        return NumericConfig;
    }

    auto FSPIRecipeV2::GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2> {
        return LiveView;
    }

} // namespace Slab::Models::StochasticPathIntegrals::V2
