//
// Created by joao on 05/09/2019.
//

#ifndef V_SHAPE_BOUNDARYCONDITIONS_H
#define V_SHAPE_BOUNDARYCONDITIONS_H

#include "Utils/Types.h"


namespace Slab::Math::Base {
    template<typename EqStateType>
    class BoundaryConditions {
        Pointer<const EqStateType> prototype;
    public:
        using EqState = EqStateType;

        explicit BoundaryConditions(Pointer<const EqState> prototype) : prototype(prototype) {};

        Pointer<EqState> newEqState() const { return prototype->clone(); }

        virtual void apply(EqState &toFunction, Real t) const = 0;
    };
}


#endif //V_SHAPE_BOUNDARYCONDITIONS_H
