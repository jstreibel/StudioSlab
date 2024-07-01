//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_FIELDSTATER2TOR_H
#define V_SHAPE_FIELDSTATER2TOR_H

#include "Models/KleinGordon/KGState.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

namespace Slab::Math::R2toR {

    class EquationState : public Models::KGState<Real2D> {
    public:
        EquationState(NumericFunction_ptr phi, NumericFunction_ptr dPhiDt)
        : Models::KGState<Real2D>(phi, dPhiDt)
        {
        }

        EqStateOutputInterface *Copy(UInt N) const override {
            throw "Not implemented";
            return nullptr;
        }

        auto replicate() const -> Pointer<Base::EquationState> override;
    };

    DefinePointers(EquationState)

}


#endif //V_SHAPE_FIELDSTATE_H
