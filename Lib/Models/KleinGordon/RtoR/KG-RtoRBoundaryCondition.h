//
// Created by joao on 30/08/2019.
//

#ifndef V_SHAPE_RTORBOUNDARYCONDITION_H
#define V_SHAPE_RTORBOUNDARYCONDITION_H


#include "Mappings/RtoR/Model/RtoRFunction.h"
#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"

#include "Math/DifferentialEquations/BoundaryConditions.h"

namespace RtoR {
    class BoundaryCondition : public Core::BoundaryConditions<RtoR::EquationState> {
    public:
        BoundaryCondition(const RtoR::EquationState &prototype,
                          Function *initialPhiCondition,
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
