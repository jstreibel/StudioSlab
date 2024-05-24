//
// Created by joao on 7/27/23.
//

#include "KG-RtoREquationState.h"

namespace Slab::Models::KGRtoR {

    auto EquationState::clone() const -> EquationState * {
        auto *clone1 = dynamic_cast<RtoR::DiscreteFunction *>(phi->Clone());
        auto *clone2 = dynamic_cast<RtoR::DiscreteFunction *>(phi->Clone());

        return new EquationState(clone1, clone2);
    }


}