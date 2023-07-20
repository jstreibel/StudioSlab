//
// Created by joao on 30/08/2019.
//

#ifndef V_SHAPE_RTORBOUNDARYCONDITION_H
#define V_SHAPE_RTORBOUNDARYCONDITION_H


#include "RtoRFunction.h"
#include "RtoRFieldState.h"

#include "Phys/DifferentialEquations/BoundaryConditions.h"

namespace RtoR {
    class BoundaryCondition : public Base::BoundaryConditions<RtoR::EquationState> {
    public:
        BoundaryCondition(Function *initialPhiCondition,
                          Function *initialdPhiDtCondition,
                          Function *leftPhiBoundaryCondition = nullptr,
                          Function *leftdPhiDtBoundaryCondition = nullptr,
                          Function *rightPhiBoundaryCondition = nullptr,
                          Function *rightdPhiDtBoundaryCondition = nullptr);

        ~BoundaryCondition();

        void apply(EquationState &fieldState, Real t) const override;

    private:
        Function *initialPhiCondition;
        Function *leftPhiBoundaryCondition;
        Function *rightPhiBoundaryCondition;

        Function *initialdPhiDtCondition;
        Function *leftdPhiDtBoundaryCondition;
        Function *rightdPhiDtBoundaryCondition;

    };
}


#endif //V_SHAPE_RTORBOUNDARYCONDITION_H
