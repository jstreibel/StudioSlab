#include "SPI-RecipeV2.h"

#include "Core/SlabCore.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Numerics/ODE/Steppers/Euler.h"
#include "Math/Numerics/V2/Listeners/ConsoleProgressListenerV2.h"
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
                               const UIntBig consoleIntervalSteps)
    : NumericConfig(numericConfig != nullptr ? numericConfig : New<SPINumericConfig>())
    , ConsoleIntervalSteps(std::max<UIntBig>(1, consoleIntervalSteps)) {
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

    auto FSPIRecipeV2::BuildSession() -> TUnique<FSimulationSessionV2> {
        if (NumericConfig == nullptr) throw Exception("FSPIRecipeV2 requires a numeric config.");

        return std::make_unique<FStepperSessionV2>(BuildStepper(),
                                                   NumericConfig->getdT(),
                                                   0,
                                                   DevFloat(0.0));
    }

    auto FSPIRecipeV2::BuildDefaultSubscriptions() -> Vector<FSubscriptionV2> {
        const auto totalSteps = NumericConfig != nullptr ? NumericConfig->Get_n() : UIntBig(0);
        auto console = New<FConsoleProgressListenerV2>(totalSteps, "SPI Console V2");

        return {{
            New<FEveryNStepsTriggerV2>(ConsoleIntervalSteps),
            console,
            EDeliveryModeV2::Synchronous,
            true,
            true
        }};
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

} // namespace Slab::Models::StochasticPathIntegrals::V2
