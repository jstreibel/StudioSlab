//
// Created by joao on 7/27/23.
//

#include "KG-RtoREquationState.h"

namespace Slab::Models::KGRtoR {

    auto EquationState::clone() const -> Pointer<Base::EquationState> {
        auto clone1 = dynamic_cast<RtoR::DiscreteFunction*>(phi->Clone());
        auto clone2 = dynamic_cast<RtoR::DiscreteFunction*>(phi->Clone());

        using Pointy = Pointer<RtoR::DiscreteFunction>;

        return New<EquationState>(Pointy(clone1), Pointy(clone2));
    }


}