//
// Created by joao on 12/24/22.
//

#ifndef STUDIOSLAB_LEADINGDELTABOUNDARYCONDITOIN_H
#define STUDIOSLAB_LEADINGDELTABOUNDARYCONDITOIN_H

#include "Phys/Numerics/Equation/BoundaryConditions.h"
#include "Fields/Mappings/R2toR/Model/FieldState.h"

#include "Fields/Mappings/R2toR/Model/FunctionsCollection/GrowingHole.h"


namespace R2toR {

    class LeadingDeltaBoundaryCondition : public Base::BoundaryConditions<R2toR::FieldState> {
        Real height;
    public:
        LeadingDeltaBoundaryCondition(Real height = 1.) : height(height) { }

        void apply(FieldState &function, Real t) const override{
            GrowingHole hole(height, t);
            GrowingHole vel(.0, t);

            function.setPhi(hole);
            function.setDPhiDt(vel);
        }
    };
}

#endif //STUDIOSLAB_LEADINGDELTABOUNDARYCONDITOIN_H
