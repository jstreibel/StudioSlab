//
// Created by joao on 7/27/23.
//

#include "KG-RtoREquationState.h"

namespace Slab::Models::KGRtoR {

    auto EquationState::replicate() const -> Pointer<Base::EquationState> {
        auto clone1 = DynamicPointerCast<RtoR::NumericFunction>(Phi->Clone());
        auto clone2 = DynamicPointerCast<RtoR::NumericFunction>(Phi->Clone());

        return New<EquationState>(clone1, clone2);
    }

    auto EquationState::category() const -> Str {
        return "2nd-order|R->R";
    }
}
