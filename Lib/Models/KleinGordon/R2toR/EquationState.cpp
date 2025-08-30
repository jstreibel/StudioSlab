//
// Created by joao on 7/27/23.
//

#include "EquationState.h"

namespace Slab::Math::R2toR {

    auto R2toR::EquationState::Replicate(std::optional<Str> Name) const -> TPointer<Base::EquationState> {
        auto clone1 = DynamicPointerCast<FNumericFunction>(Phi->Clone());
        if (Name) clone1->change_data_name(Name.value() + " (eq.1)");

        auto clone2 = DynamicPointerCast<FNumericFunction>(Phi->Clone());
        if (Name) clone2->change_data_name(Name.value() + " (eq.2)");

        return New<R2toR::EquationState>(clone1, clone2);
    }

    auto EquationState::category() const -> Str {
        return "2nd-order|R2->R";
    }
}
