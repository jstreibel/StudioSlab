//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_FIELDSTATER2TOR_H
#define V_SHAPE_FIELDSTATER2TOR_H

#include "Phys/DifferentialEquations/2nd-Order/GordonStateT.h"
#include "R2toRDiscreteFunction.h"

namespace R2toR {
    class EquationState : public Phys::Gordon::GordonStateT<DiscreteFunction> {
    public:
        EquationState(DiscreteFunction *phi, DiscreteFunction *dPhiDt)
        : Phys::Gordon::GordonStateT<DiscreteFunction>(phi, dPhiDt)
        {
        }

        FStateOutputInterface *Copy(PosInt N) const override {
            throw "Not implemented";
            return nullptr;
        }

        auto clone() const -> EquationState *;
    };

}


#endif //V_SHAPE_FIELDSTATE_H
