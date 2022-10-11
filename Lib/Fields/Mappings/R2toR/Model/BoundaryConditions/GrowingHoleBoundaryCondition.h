//
// Created by joao on 10/11/22.
//

#ifndef STUDIOSLAB_GROWINGHOLEBOUNDARYCONDITION_H
#define STUDIOSLAB_GROWINGHOLEBOUNDARYCONDITION_H


#include "Phys/Numerics/Equation/BoundaryConditions.h"
#include "Fields/Mappings/R2toR/Model/FieldState.h"

namespace R2toR {

    class GrowingHoleBoundaryCondition : public Base::BoundaryConditions<R2toR::FieldState> {
        Real height;
    public:
        GrowingHoleBoundaryCondition(Real height = 1.);

        void apply(FieldState &function, Real t) const override;

    };
}

#endif //STUDIOSLAB_GROWINGHOLEBOUNDARYCONDITION_H
