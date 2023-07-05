//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TORBOUNDARYCONDITION_H
#define V_SHAPE_R2TORBOUNDARYCONDITION_H

#include "Mappings/R2toR/Model/EquationState.h"
#include "Phys/DifferentialEquations/BoundaryConditions.h"


namespace R2toR {

    class BoundaryCondition : public Base::BoundaryConditions<R2toR::EquationState> {
    public:
        BoundaryCondition(Function *initialPhiCondition,
                          Function *initialdPhiDtCondition);

        void apply(EquationState &fieldState, Real t) const override;

    private:
        Function *initialPhiCondition;
        Function *initialdPhiDtCondition;

        Function *phi, *dPhiDt;

    };

}


#endif //V_SHAPE_R2TORBOUNDARYCONDITION_H
