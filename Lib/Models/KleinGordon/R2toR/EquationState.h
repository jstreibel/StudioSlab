//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_FIELDSTATER2TOR_H
#define V_SHAPE_FIELDSTATER2TOR_H

#include "Models/KleinGordon/KGState.h"
#include "Mappings/R2toR/Model/R2toRDiscreteFunction.h"

namespace R2toR {
    class EquationState : public Fields::KleinGordon::KGState<DiscreteFunction> {
    public:
        EquationState(DiscreteFunction *phi, DiscreteFunction *dPhiDt)
        : Fields::KleinGordon::KGState<DiscreteFunction>(phi, dPhiDt)
        {
        }

        EqStateOutputInterface *Copy(PosInt N) const override {
            throw "Not implemented";
            return nullptr;
        }

        auto clone() const -> EquationState *;
    };

}


#endif //V_SHAPE_FIELDSTATE_H
