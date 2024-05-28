//
// Created by joao on 7/27/23.
//

#include "EquationState.h"

namespace Slab::Math::R2toR {

    auto R2toR::EquationState::clone() const -> Pointer<Base::EquationState> {
        auto clone1 = DynamicPointerCast<DiscreteFunction>(phi->Clone());
        auto clone2 = DynamicPointerCast<DiscreteFunction>(phi->Clone());

        return New<R2toR::EquationState>(clone1, clone2);
    }

}