//
// Created by joao on 30/08/2019.
//

#include "KG-RtoRBoundaryCondition.h"


namespace Slab::Models::KGRtoR {


    BoundaryCondition::BoundaryCondition(const EquationState_constptr &prototype,
                                               RtoR::Function *initialPhiCondition,
                                               RtoR::Function *initialdPhiDtCondition,
                                               RtoR::Function *leftPhiBoundaryCondition,
                                               RtoR::Function *leftdPhiDtBoundaryCondition,
                                               RtoR::Function *rightPhiBoundaryCondition,
                                               RtoR::Function *rightdPhiDtBoundaryCondition)
    : Base::BoundaryConditions(prototype), initialPhiCondition(initialPhiCondition)
    , leftPhiBoundaryCondition(leftPhiBoundaryCondition), rightPhiBoundaryCondition(rightPhiBoundaryCondition)
    , initialdPhiDtCondition(initialdPhiDtCondition), leftdPhiDtBoundaryCondition(leftdPhiDtBoundaryCondition)
    , rightdPhiDtBoundaryCondition(rightdPhiDtBoundaryCondition) { }


    void BoundaryCondition::applyKG(EquationState &kgState, Real t) const {
        if (t == 0.0) {
            kgState.setPhi(*initialPhiCondition);
            kgState.setDPhiDt(*initialdPhiDtCondition);
        } else {
            kgState.getPhi();

            if (rightPhiBoundaryCondition != nullptr ||
                rightdPhiDtBoundaryCondition != nullptr ||
                leftPhiBoundaryCondition != nullptr ||
                leftdPhiDtBoundaryCondition != nullptr)
                throw "Not implemented. But definitely very simple to implement.";
        }
    };

    void BoundaryCondition::apply(Base::EquationState &state, const floatt t) const {
        auto kgState = dynamic_cast<EquationState&>(state);

        this->applyKG(kgState, t);

    }

    BoundaryCondition::~BoundaryCondition() {
        delete initialPhiCondition;
        delete initialdPhiDtCondition;

        delete leftPhiBoundaryCondition;      // Null ptr gets deleted anyway
        delete leftdPhiDtBoundaryCondition;
        delete rightPhiBoundaryCondition;
        delete rightdPhiDtBoundaryCondition;
    }

}