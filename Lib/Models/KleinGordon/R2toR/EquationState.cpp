//
// Created by joao on 7/27/23.
//

#include "EquationState.h"

namespace Slab::Math::R2toR {

    auto R2toR::EquationState::replicate() const -> Pointer<Base::EquationState> {
        auto clone1 = DynamicPointerCast<NumericFunction>(phi->Clone());
        auto clone2 = DynamicPointerCast<NumericFunction>(phi->Clone());

        return New<R2toR::EquationState>(clone1, clone2);
    }

    auto EquationState::category() const -> Str {
        return "2nd-order|R2->R";
    }
}
