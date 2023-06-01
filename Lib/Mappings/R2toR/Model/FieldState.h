//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_FIELDSTATER2TOR_H
#define V_SHAPE_FIELDSTATER2TOR_H

#include "Phys/DifferentialEquations/2nd-Order/FieldState.h"
#include "R2toRFunctionArbitrary.h"

namespace R2toR {
    class FieldState : public Base::FieldState<FunctionArbitrary> {
    public:
        FieldState(FunctionArbitrary *phi, FunctionArbitrary *dPhiDt) : Base::FieldState<FunctionArbitrary>(phi, dPhiDt)
        {
        }

        FStateOutputInterface *Copy(PosInt N) const override {
            throw "Not implemented";
            return nullptr;
        }
    };
}


#endif //V_SHAPE_FIELDSTATE_H
