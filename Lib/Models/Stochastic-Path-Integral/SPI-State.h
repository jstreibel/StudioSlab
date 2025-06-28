//
// Created by joao on 25/03/25.
//

#ifndef SPI_STATE_H
#define SPI_STATE_H

#include <Math/Function/R2toR/Model/R2toRNumericFunction.h>
#include "Math/Numerics/ODE/Solver/EquationState.h"

namespace Slab::Models::StochasticPathIntegrals {

    class SPIState final : public Math::Base::EquationState {
        Pointer<Math::R2toR::NumericFunction> data;
    public:
        explicit SPIState(Pointer<Math::R2toR::NumericFunction> data);

        EquationState & StoreAddition(const EquationState &, const EquationState &) override;

        EquationState & StoreSubtraction(const EquationState &, const EquationState &) override;

        EquationState & Add(const EquationState &) override;

        EquationState & Subtract(const EquationState &) override;

        EquationState & StoreScalarMultiplication(const EquationState &, DevFloat a) override;

        EquationState & Multiply(DevFloat a) override;

        [[nodiscard]] auto category() const -> Str override;

        [[nodiscard]] auto replicate() const -> Pointer<EquationState> override;

        [[nodiscard]] auto getPhi() const -> Pointer<Math::R2toR::NumericFunction>;
        [[nodiscard]] auto getPhi() -> Pointer<Math::R2toR::NumericFunction>;

        auto setData(const EquationState &) -> void override;
        [[nodiscard]] auto cloneData() const -> Pointer<Math::R2toR::NumericFunction>;
    };

} // StochasticPathIntegral::Models::Slab

#endif //SPI_STATE_H
