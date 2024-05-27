//
// Created by joao on 7/27/23.
//

#include "EquationState.h"

namespace Slab::Math::R2toR {

    auto R2toR::EquationState::clone() const -> Pointer<Base::EquationState> {
        auto *clone1 = dynamic_cast<R2toR::DiscreteFunction *>(phi->Clone());
        auto *clone2 = dynamic_cast<R2toR::DiscreteFunction *>(phi->Clone());

        using Pointy = Pointer<R2toR::DiscreteFunction>;

        return New<R2toR::EquationState>(Pointy(clone1), Pointy(clone2));
    }

}