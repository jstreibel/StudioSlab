//
// Created by joao on 23/09/2019.
//

#include "R2ToRBoundaryCondition.h"

namespace Slab::Math {

    R2toR::BoundaryCondition::BoundaryCondition(R2toR::Function *initialPhiCondition,
                                                R2toR::Function *initialdPhiDtCondition,
                                                const R2toR::EquationState_constptr& prototype)
    : Base::BoundaryConditions(prototype)
    , initialPhiCondition(initialPhiCondition)
    , initialdPhiDtCondition(initialdPhiDtCondition)
    {
    }

    void R2toR::BoundaryCondition::apply(Base::EquationState &state, Real t) const {
        auto kgState = dynamic_cast<EquationState&>(state);

        this->apply_KGR2toR(kgState, t);
    }


    void R2toR::BoundaryCondition::apply_KGR2toR(EquationState &fieldState, Real t) const {
        if (t == 0.0) {
            fieldState.setPhi(*initialPhiCondition);
            fieldState.setDPhiDt(*initialdPhiDtCondition);
        } else {

        }
    }


}