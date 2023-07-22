//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_FIELDSTATER2TOR_H
#define V_SHAPE_FIELDSTATER2TOR_H

#include "Phys/DifferentialEquations/2nd-Order/GordonStateT.h"
#include "R2toRDiscreteFunction.h"

namespace R2toR {
    class EquationState : public Base::EquationStateT<DiscreteFunction> {
    public:
        EquationState(DiscreteFunction *phi, DiscreteFunction *dPhiDt) : Base::EquationStateT<DiscreteFunction>(phi, dPhiDt)
        {
        }

        FStateOutputInterface *Copy(PosInt N) const override {
            throw "Not implemented";
            return nullptr;
        }
    };
}


#endif //V_SHAPE_FIELDSTATE_H
