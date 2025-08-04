//
// Created by joao on 30/08/2019.
//

#include "KG-RtoRBoundaryCondition.h"


namespace Slab::Models::KGRtoR {


    BoundaryCondition::BoundaryCondition(const EquationState_constptr &prototype,
                                               RtoR::Function_ptr initialPhiCondition,
                                               RtoR::Function_ptr initialdPhiDtCondition,
                                               RtoR::Function_ptr leftPhiBoundaryCondition,
                                               RtoR::Function_ptr leftdPhiDtBoundaryCondition,
                                               RtoR::Function_ptr rightPhiBoundaryCondition,
                                               RtoR::Function_ptr rightdPhiDtBoundaryCondition)
    : Base::BoundaryConditions(prototype), initialPhiCondition(initialPhiCondition)
    , leftPhiBoundaryCondition(leftPhiBoundaryCondition), rightPhiBoundaryCondition(rightPhiBoundaryCondition)
    , initialdPhiDtCondition(initialdPhiDtCondition), leftdPhiDtBoundaryCondition(leftdPhiDtBoundaryCondition)
    , rightdPhiDtBoundaryCondition(rightdPhiDtBoundaryCondition) { }


    void BoundaryCondition::ApplyKG(EquationState &kgState, Real t) const {
        if (t == 0.0) {
            kgState.SetPhi(*initialPhiCondition);
            kgState.SetDPhiDt(*initialdPhiDtCondition);
        } else {
            kgState.getPhi();

            if (rightPhiBoundaryCondition != nullptr ||
                rightdPhiDtBoundaryCondition != nullptr ||
                leftPhiBoundaryCondition != nullptr ||
                leftdPhiDtBoundaryCondition != nullptr)
                throw NotImplementedException("Not implemented. But definitely very simple to implement.");
        }
    }

    void BoundaryCondition::Apply(Base::EquationState &state, const floatt t) const {
        auto &KGState = dynamic_cast<EquationState&>(state);

        this->ApplyKG(KGState, t);

    }

    BoundaryCondition::~BoundaryCondition() = default;

}