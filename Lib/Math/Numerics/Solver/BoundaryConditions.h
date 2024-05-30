//
// Created by joao on 05/09/2019.
//

#ifndef V_SHAPE_BOUNDARYCONDITIONS_H
#define V_SHAPE_BOUNDARYCONDITIONS_H

#include <utility>
#include "Utils/Types.h"
#include "EquationState.h"

namespace Slab::Math::Base {

    class BoundaryConditions {
        EquationState_constptr prototype;

    public:

        explicit BoundaryConditions(EquationState_constptr prototype);

        EquationState_ptr newEqState() const;

        virtual void apply(EquationState &toFunction, Real t) const = 0;
    };

    DefinePointer(BoundaryConditions)
}


#endif //V_SHAPE_BOUNDARYCONDITIONS_H
