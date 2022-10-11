//
// Created by joao on 23/09/2019.
//

#include "R2ToRBoundaryCondition.h"


R2toR::BoundaryCondition::BoundaryCondition(R2toR::Function *initialPhiCondition,
                                            R2toR::Function *initialdPhiDtCondition)
                                            : initialPhiCondition(initialPhiCondition),
                                              initialdPhiDtCondition(initialdPhiDtCondition){
}

void R2toR::BoundaryCondition::apply(FieldState &fieldState, Real t) const {
    if(t == 0.0){
        fieldState.setPhi(*initialPhiCondition);
        fieldState.setDPhiDt(*initialdPhiDtCondition);
    } else {

    }
}
