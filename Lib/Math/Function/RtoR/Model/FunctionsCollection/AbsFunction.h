//
// Created by joao on 11/09/2019.
//

#ifndef V_SHAPE_ABSFUNCTION_H
#define V_SHAPE_ABSFUNCTION_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {
    class AbsFunction final : public RtoR::Function {
    public:
        DevFloat operator()(DevFloat x) const override;

        TPointer<FunctionT> diff(int n) const override;

        TPointer<FunctionT<DevFloat, DevFloat>> Clone() const override;

        Str Symbol() const override;
    };
}

#endif //V_SHAPE_ABSFUNCTION_H
