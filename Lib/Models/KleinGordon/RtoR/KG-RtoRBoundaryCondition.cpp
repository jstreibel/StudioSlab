//
// Created by joao on 30/08/2019.
//

#include "KG-RtoRBoundaryCondition.h"


namespace Slab::Models::KGRtoR {


    BoundaryCondition::BoundaryCondition(const EquationState &prototype,
                                               RtoR::Function *initialPhiCondition,
                                               RtoR::Function *initialdPhiDtCondition,
                                               RtoR::Function *leftPhiBoundaryCondition,
                                               RtoR::Function *leftdPhiDtBoundaryCondition,
                                               RtoR::Function *rightPhiBoundaryCondition,
                                               RtoR::Function *rightdPhiDtBoundaryCondition)
            : Base::BoundaryConditions<EquationState>(prototype), initialPhiCondition(initialPhiCondition),
              leftPhiBoundaryCondition(leftPhiBoundaryCondition), rightPhiBoundaryCondition(rightPhiBoundaryCondition),
              initialdPhiDtCondition(initialdPhiDtCondition), leftdPhiDtBoundaryCondition(leftdPhiDtBoundaryCondition),
              rightdPhiDtBoundaryCondition(rightdPhiDtBoundaryCondition) {}

    void BoundaryCondition::apply(EquationState &fieldState, const floatt t) const {
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

    BoundaryCondition::~BoundaryCondition() {
        delete initialPhiCondition;
        delete initialdPhiDtCondition;

        delete leftPhiBoundaryCondition;      // Null ptr gets deleted anyway
        delete leftdPhiDtBoundaryCondition;
        delete rightPhiBoundaryCondition;
        delete rightdPhiDtBoundaryCondition;
    };


}