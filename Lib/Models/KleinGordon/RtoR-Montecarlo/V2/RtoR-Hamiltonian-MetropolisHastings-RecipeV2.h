#ifndef STUDIOSLAB_RTOR_HAMILTONIAN_METROPOLISHASTINGS_RECIPE_V2_H
#define STUDIOSLAB_RTOR_HAMILTONIAN_METROPOLISHASTINGS_RECIPE_V2_H

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Math/Numerics/Stepper.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

namespace Slab::Models::KGRtoR::Metropolis::V2 {

    class FRtoRHamiltonianMetropolisHastingsRecipeV2 final : public Math::Numerics::V2::FSimulationRecipeV2 {
        UIntBig MaxSteps = 0;
        UIntBig ConsoleIntervalSteps = 1;
        UIntBig LiveViewIntervalSteps = 0; // 0 => follow ConsoleIntervalSteps
        bool bRunEndless = false;

        TPointer<Math::RtoR::NumericFunction_CPU> PhiField = nullptr;
        TPointer<Math::RtoR::NumericFunction_CPU> PiField = nullptr;
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView = nullptr;

        auto BuildStepper() -> TPointer<Math::FStepper>;

    public:
        explicit FRtoRHamiltonianMetropolisHastingsRecipeV2(UIntBig maxSteps,
                                                            UIntBig consoleIntervalSteps = 1000,
                                                            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr,
                                                            bool bRunEndless = false);

        auto BuildSession() -> TPointer<Math::Numerics::V2::FSimulationSessionV2> override;
        auto BuildDefaultSubscriptions() -> Vector<Math::Numerics::V2::FSubscriptionV2> override;

        auto SetLiveViewIntervalSteps(UIntBig liveViewIntervalSteps) -> void;

        [[nodiscard]] auto GetRunLimits() const -> Math::Numerics::V2::FRunLimitsV2 override;

        [[nodiscard]] auto GetPhiField() const -> TPointer<Math::RtoR::NumericFunction_CPU>;
        [[nodiscard]] auto GetPiField() const -> TPointer<Math::RtoR::NumericFunction_CPU>;
        [[nodiscard]] auto GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2>;
    };

    DefinePointers(FRtoRHamiltonianMetropolisHastingsRecipeV2)

} // namespace Slab::Models::KGRtoR::Metropolis::V2

#endif // STUDIOSLAB_RTOR_HAMILTONIAN_METROPOLISHASTINGS_RECIPE_V2_H
