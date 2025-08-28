//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_FIELDSTATER2TOR_H
#define V_SHAPE_FIELDSTATER2TOR_H

#include "Models/KleinGordon/KG-State.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

namespace Slab::Math::R2toR {

    class EquationState : public Models::KGState<Real2D> {
    public:
        EquationState(const FNumericFunction_ptr &phi, const FNumericFunction_ptr &dPhiDt)
        : Models::KGState<Real2D>(phi, dPhiDt)
        {
        }

        [[nodiscard]] EqStateOutputInterface *Copy(UInt N) const override {
            NOT_IMPLEMENTED_CLASS_METHOD
        }

        [[nodiscard]] auto Replicate(std::optional<Str> Name) const -> TPointer<Base::EquationState> override;

        [[nodiscard]] auto category() const -> Str override;
    };

    DefinePointers(EquationState)

}


#endif //V_SHAPE_FIELDSTATE_H
