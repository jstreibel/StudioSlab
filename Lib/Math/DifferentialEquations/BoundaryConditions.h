//
// Created by joao on 05/09/2019.
//

#ifndef V_SHAPE_BOUNDARYCONDITIONS_H
#define V_SHAPE_BOUNDARYCONDITIONS_H

#include "Utils/Types.h"


namespace Core {
    template<typename EqStateType>
    class BoundaryConditions {
        const EqStateType &prototype;
    public:
        using EqState = EqStateType;

        explicit BoundaryConditions(const EqState &prototype) : prototype(prototype) {};

        EqState *newEqState() const { return prototype.clone(); }

        virtual void apply(EqState &toFunction, Real t) const = 0;
    };
}


#endif //V_SHAPE_BOUNDARYCONDITIONS_H
