//
// Created by joao on 11/09/2019.
//

#ifndef V_SHAPE_ABSFUNCTION_H
#define V_SHAPE_ABSFUNCTION_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {
    class AbsFunction final : public RtoR::Function {
    public:
        Real operator()(Real x) const override;

        Pointer<FunctionT> diff(int n) const override;

        Pointer<FunctionT<Real, Real>> Clone() const override;

        Str symbol() const override;
    };
}

#endif //V_SHAPE_ABSFUNCTION_H
