//
// Created by joao on 10/11/22.
//

#include "GrowingHoleBoundaryCondition.h"
#include "Fields/Mappings/R2toR/Model/FunctionsCollection/GrowingHole.h"

R2toR::GrowingHoleBoundaryCondition::GrowingHoleBoundaryCondition(Real height) : height(height) {

}

void R2toR::GrowingHoleBoundaryCondition::apply(R2toR::FieldState &function, Real t) const {
    GrowingHole hole(height, t);
    GrowingHole vel(.0, t);

    function.setPhi(hole);
    function.setDPhiDt(vel);
}


