//
// Created by joao on 10/11/22.
//

#ifndef STUDIOSLAB_GROWINGHOLEBOUNDARYCONDITION_H
#define STUDIOSLAB_GROWINGHOLEBOUNDARYCONDITION_H


#include "Phys/DifferentialEquations/BoundaryConditions.h"
#include "Mappings/R2toR/Model/EquationState.h"

#include "Mappings/R2toR/Model/FunctionsCollection/GrowingHole.h"


namespace R2toR {

    namespace GrowingHole {

        class GrowingHoleBoundaryCondition : public Base::BoundaryConditions<R2toR::EquationState> {
            Real height;
        public:
            GrowingHoleBoundaryCondition(Real height = 2 / 3.) : height(height) {}

            void apply(EquationState &function, Real t) const override {
                GrowingHoleFunc hole(height, t);
                GrowingHoleFunc vel(.0, t);

                function.setPhi(hole);
                function.setDPhiDt(vel);
            }
        };
    }
}


#endif //STUDIOSLAB_GROWINGHOLEBOUNDARYCONDITION_H
