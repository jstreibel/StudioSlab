//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TORBOUNDARYCONDITION_H
#define V_SHAPE_R2TORBOUNDARYCONDITION_H

#include "FieldState.h"
#include "Studios/Math/System/BoundaryConditions.h"


namespace R2toR {

    class BoundaryCondition : public Base::BoundaryConditions<R2toR::FieldState> {
    public:
        BoundaryCondition(Function *initialPhiCondition,
                          Function *initialdPhiDtCondition);

        void apply(FieldState &fieldState, Real t) const override;

    private:
        Function *initialPhiCondition;
        Function *initialdPhiDtCondition;

    };

}


#endif //V_SHAPE_R2TORBOUNDARYCONDITION_H
