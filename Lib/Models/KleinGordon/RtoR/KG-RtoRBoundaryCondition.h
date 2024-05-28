//
// Created by joao on 30/08/2019.
//

#ifndef V_SHAPE_RTORBOUNDARYCONDITION_H
#define V_SHAPE_RTORBOUNDARYCONDITION_H


#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"

#include "Math/DifferentialEquations/BoundaryConditions.h"

namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    class BoundaryCondition : public Base::BoundaryConditions {
        RtoR::Function *initialPhiCondition;
        RtoR::Function *leftPhiBoundaryCondition;
        RtoR::Function *rightPhiBoundaryCondition;

        RtoR::Function *initialdPhiDtCondition;
        RtoR::Function *leftdPhiDtBoundaryCondition;
        RtoR::Function *rightdPhiDtBoundaryCondition;

    protected:

        virtual void applyKG(EquationState &kgState, Real t) const;

    public:
        BoundaryCondition(const KGRtoR::EquationState_constptr &prototype,
                          RtoR::Function *initialPhiCondition,
                          RtoR::Function *initialdPhiDtCondition,
                          RtoR::Function *leftPhiBoundaryCondition = nullptr,
                          RtoR::Function *leftdPhiDtBoundaryCondition = nullptr,
                          RtoR::Function *rightPhiBoundaryCondition = nullptr,
                          RtoR::Function *rightdPhiDtBoundaryCondition = nullptr);

        ~BoundaryCondition();

        void apply(Base::EquationState &state, Real t) const final;



    };

    DefinePointer(BoundaryCondition)
}


#endif //V_SHAPE_RTORBOUNDARYCONDITION_H
