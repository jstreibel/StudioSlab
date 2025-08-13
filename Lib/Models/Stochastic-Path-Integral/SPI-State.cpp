//
// Created by joao on 25/03/25.
//

#include "SPI-State.h"

namespace Slab::Models::StochasticPathIntegrals {
    SPIState::SPIState(TPointer<Math::R2toR::FNumericFunction> data): data(std::move(data)) {}

#define MathCast(a) DynamicPointerCast<Math::R2toR::FNumericFunction>((a))
#define MyCast(a) dynamic_cast<const SPIState&>((a))
#define GetData(a) (*(MyCast(a).getPhi()))

    Math::Base::EquationState & SPIState::StoreAddition(const EquationState &a, const EquationState &b) {
        data->StoreAddition(GetData(a), GetData(b));
        return *this;
    }

    Math::Base::EquationState & SPIState::StoreSubtraction(const EquationState &a, const EquationState &b) {
        data->StoreSubtraction(GetData(a), GetData(b));
        return *this;
    }

    Math::Base::EquationState & SPIState::Add(const EquationState &a) {
        data->Add(GetData(a));
        return *this;
    }

    Math::Base::EquationState & SPIState::Subtract(const EquationState &a) {
        data->Subtract(GetData(a));
        return *this;
    }

    Math::Base::EquationState & SPIState::StoreScalarMultiplication(const EquationState &a, DevFloat c) {
        data->StoreScalarMultiplication(GetData(a), c);
        return *this;
    }

    Math::Base::EquationState & SPIState::Multiply(DevFloat a) {
        data->Multiply(a);
        return *this;
    }

    auto SPIState::category() const -> Str {
        return "1st-order|R2->R";
    }

    auto SPIState::replicate() const -> TPointer<EquationState> {
        return New<SPIState>(MathCast(data->Clone()));
    }

    auto SPIState::getPhi() const -> TPointer<Math::R2toR::FNumericFunction> {
        return data;
    }

    auto SPIState::getPhi() -> TPointer<Math::R2toR::FNumericFunction> {
        return data;
    }

    void SPIState::setData(const EquationState &a) {
        auto brother_state = MyCast(a);
        auto numeric_data =
            DynamicPointerCast<Math::R2toR::FNumericFunction::MyType>(brother_state.data);

        data->SetArb(*numeric_data);
    }

    auto SPIState::cloneData() const -> TPointer<Math::R2toR::FNumericFunction> {
        return DynamicPointerCast<Math::R2toR::FNumericFunction>(data->Clone());
    }
} // Slab::Models::StochasticPathIntegral