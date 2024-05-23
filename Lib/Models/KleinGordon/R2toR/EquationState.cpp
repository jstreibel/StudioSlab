//
// Created by joao on 7/27/23.
//

#include "EquationState.h"

namespace Slab::Math::R2toR {

    auto R2toR::EquationState::clone() const -> R2toR::EquationState * {
        auto *clone1 = dynamic_cast<R2toR::DiscreteFunction *>(phi->Clone());
        auto *clone2 = dynamic_cast<R2toR::DiscreteFunction *>(phi->Clone());

        return new R2toR::EquationState(clone1, clone2);
    }

}