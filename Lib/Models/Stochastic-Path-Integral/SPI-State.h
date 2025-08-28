//
// Created by joao on 25/03/25.
//

#ifndef SPI_STATE_H
#define SPI_STATE_H

#include <Math/Function/R2toR/Model/R2toRNumericFunction.h>
#include "Math/Numerics/ODE/Solver/EquationState.h"

namespace Slab::Models::StochasticPathIntegrals {

    class SPIState final : public Math::Base::EquationState {
        TPointer<Math::R2toR::FNumericFunction> data;
    public:
        explicit SPIState(TPointer<Math::R2toR::FNumericFunction> data);

        EquationState & StoreAddition(const EquationState &, const EquationState &) override;

        EquationState & StoreSubtraction(const EquationState &, const EquationState &) override;

        EquationState & Add(const EquationState &) override;

        EquationState & Subtract(const EquationState &) override;

        EquationState & StoreScalarMultiplication(const EquationState &, DevFloat a) override;

        EquationState & Multiply(DevFloat a) override;

        [[nodiscard]] auto category() const -> Str override;

        [[nodiscard]] auto Replicate(std::optional<Str> Name) const -> TPointer<EquationState> override;

        [[nodiscard]] auto getPhi() const -> TPointer<Math::R2toR::FNumericFunction>;
        [[nodiscard]] auto getPhi() -> TPointer<Math::R2toR::FNumericFunction>;

        auto setData(const EquationState &) -> void override;
        [[nodiscard]] auto cloneData() const -> TPointer<Math::R2toR::FNumericFunction>;
    };

} // StochasticPathIntegral::Models::Slab

#endif //SPI_STATE_H
