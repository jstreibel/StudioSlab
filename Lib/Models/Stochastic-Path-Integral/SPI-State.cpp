//
// Created by joao on 25/03/25.
//

#include "SPI-State.h"

namespace Slab::Models::StochasticPathIntegrals {
    SPIState::SPIState(Pointer<Math::R2toR::NumericFunction> data): data(std::move(data)) {}

#define MyCast(a) dynamic_cast<const Math::R2toR::NumericFunction&>((a))

    Math::Base::EquationState & SPIState::StoreAddition(const EquationState &a, const EquationState &b) {
        data->StoreAddition(MyCast(a), MyCast(b));
        return *this;
    }

    Math::Base::EquationState & SPIState::StoreSubtraction(const EquationState &a, const EquationState &b) {
        data->StoreSubtraction(MyCast(a), MyCast(b));
        return *this;
    }

    Math::Base::EquationState & SPIState::Add(const EquationState &a) {
        data->Add(MyCast(a));
        return *this;
    }

    Math::Base::EquationState & SPIState::Subtract(const EquationState &a) {
        data->Subtract(MyCast(a));
        return *this;
    }

    Math::Base::EquationState & SPIState::StoreScalarMultiplication(const EquationState &a, Real c) {
        data->StoreScalarMultiplication(MyCast(a), c);
        return *this;
    }

    Math::Base::EquationState & SPIState::Multiply(Real a) {
        data->Multiply(a);
        return *this;
    }

    auto SPIState::replicate() const -> Pointer<EquationState> {
        return New<SPIState>(DynamicPointerCast<Math::R2toR::NumericFunction>(data->Clone()));
    }

    auto SPIState::getPhi() const -> Pointer<Math::R2toR::NumericFunction> {
        return data;
    }

    auto SPIState::getPhi() -> Pointer<Math::R2toR::NumericFunction> {
        return data;
    }

    void SPIState::setData(const EquationState &a) {
        auto brother_state = dynamic_cast<const SPIState&>(a);
        auto numeric_data =
            DynamicPointerCast<Math::R2toR::NumericFunction::MyType>(brother_state.data);

        data->SetArb(*numeric_data);
    }

    auto SPIState::cloneData() const -> Pointer<Math::R2toR::NumericFunction> {
        return DynamicPointerCast<Math::R2toR::NumericFunction>(data->Clone());
    }
} // Slab::Models::StochasticPathIntegral