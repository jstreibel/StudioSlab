//
// Created by joao on 30/08/2019.
//

#include "KG-RtoRBoundaryCondition.h"


namespace Slab::Math::RtoR {


    RtoR::BoundaryCondition::BoundaryCondition(const RtoR::EquationState &prototype,
                                               Function *initialPhiCondition,
                                               Function *initialdPhiDtCondition,
                                               Function *leftPhiBoundaryCondition,
                                               Function *leftdPhiDtBoundaryCondition,
                                               Function *rightPhiBoundaryCondition,
                                               Function *rightdPhiDtBoundaryCondition)
            : Base::BoundaryConditions<RtoR::EquationState>(prototype), initialPhiCondition(initialPhiCondition),
              leftPhiBoundaryCondition(leftPhiBoundaryCondition), rightPhiBoundaryCondition(rightPhiBoundaryCondition),
              initialdPhiDtCondition(initialdPhiDtCondition), leftdPhiDtBoundaryCondition(leftdPhiDtBoundaryCondition),
              rightdPhiDtBoundaryCondition(rightdPhiDtBoundaryCondition) {}

    void RtoR::BoundaryCondition::apply(EquationState &fieldState, const floatt t) const {
        if (t == 0.0) {
            fieldState.setPhi(*initialPhiCondition);
            fieldState.setDPhiDt(*initialdPhiDtCondition);
        } else {
            fieldState.getPhi();

            if (rightPhiBoundaryCondition != nullptr ||
                rightdPhiDtBoundaryCondition != nullptr ||
                leftPhiBoundaryCondition != nullptr ||
                leftdPhiDtBoundaryCondition != nullptr)
                throw "Not implemented. But definitely very simple to implement.";
        }
    }

    RtoR::BoundaryCondition::~BoundaryCondition() {
        delete initialPhiCondition;
        delete initialdPhiDtCondition;

        delete leftPhiBoundaryCondition;      // Null ptr gets deleted anyway
        delete leftdPhiDtBoundaryCondition;
        delete rightPhiBoundaryCondition;
        delete rightdPhiDtBoundaryCondition;
    };


}