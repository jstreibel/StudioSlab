//
// Created by joao on 27/05/24.
//

#include "BoundaryConditions.h"


namespace Slab::Math::Base {

    BoundaryConditions::BoundaryConditions(EquationState_constptr prototype)
    : prototype(std::move(prototype)) {}

    EquationState_ptr BoundaryConditions::newEqState() const { return prototype->replicate(); }


}