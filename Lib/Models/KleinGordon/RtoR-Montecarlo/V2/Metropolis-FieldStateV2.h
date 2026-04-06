#ifndef STUDIOSLAB_METROPOLIS_FIELD_STATE_V2_H
#define STUDIOSLAB_METROPOLIS_FIELD_STATE_V2_H

#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Math/Numerics/ODE/Solver/EquationState.h"

namespace Slab::Models::KGRtoR::Metropolis::V2 {

    class FMetropolisFieldStateV2 final : public Math::Base::EquationState {
        TPointer<Math::RtoR::NumericFunction_CPU> PhiField = nullptr;
        TPointer<Math::RtoR::NumericFunction_CPU> PiField = nullptr;

        static auto Cast(const Math::Base::EquationState &state) -> const FMetropolisFieldStateV2 &;
        static auto Cast(Math::Base::EquationState &state) -> FMetropolisFieldStateV2 &;
        auto EnsureCompatible(const FMetropolisFieldStateV2 &other) const -> void;

    public:
        FMetropolisFieldStateV2(const TPointer<Math::RtoR::NumericFunction_CPU> &phiField,
                                const TPointer<Math::RtoR::NumericFunction_CPU> &piField);

        [[nodiscard]] auto GetPhiField() const -> TPointer<Math::RtoR::NumericFunction_CPU>;
        [[nodiscard]] auto GetPiField() const -> TPointer<Math::RtoR::NumericFunction_CPU>;

        [[nodiscard]] auto Replicate(std::optional<Str> name) const -> TPointer<Math::Base::EquationState> override;
        [[nodiscard]] auto category() const -> Str override;

        auto setData(const Math::Base::EquationState &state) -> void override;
        auto Add(const Math::Base::EquationState &state) -> Math::Base::EquationState & override;
        auto Subtract(const Math::Base::EquationState &state) -> Math::Base::EquationState & override;
        auto StoreAddition(const Math::Base::EquationState &state1,
                           const Math::Base::EquationState &state2) -> Math::Base::EquationState & override;
        auto StoreSubtraction(const Math::Base::EquationState &state1,
                              const Math::Base::EquationState &state2) -> Math::Base::EquationState & override;
        auto StoreScalarMultiplication(const Math::Base::EquationState &state,
                                       DevFloat a) -> Math::Base::EquationState & override;
        auto Multiply(DevFloat a) -> Math::Base::EquationState & override;
    };

    DefinePointers(FMetropolisFieldStateV2)

} // namespace Slab::Models::KGRtoR::Metropolis::V2

#endif // STUDIOSLAB_METROPOLIS_FIELD_STATE_V2_H
