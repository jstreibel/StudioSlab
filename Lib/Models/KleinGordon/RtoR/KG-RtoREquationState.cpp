//
// Created by joao on 7/27/23.
//

#include "KG-RtoREquationState.h"

namespace Slab::Math {

    auto RtoR::EquationState::clone() const -> RtoR::EquationState * {
        auto *clone1 = dynamic_cast<RtoR::DiscreteFunction *>(phi->Clone());
        auto *clone2 = dynamic_cast<RtoR::DiscreteFunction *>(phi->Clone());

        return new RtoR::EquationState(clone1, clone2);
    }


}