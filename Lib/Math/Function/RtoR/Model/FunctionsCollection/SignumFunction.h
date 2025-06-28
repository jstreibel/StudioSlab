//
// Created by joao on 11/09/2019.
//

#ifndef V_SHAPE_SIGNUMFUNCTION_H
#define V_SHAPE_SIGNUMFUNCTION_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

#include "Math/Function/GPUFriendly.h"

namespace Slab::Math::RtoR {
    class SignumFunction : public RtoR::Function, Base::GPUFriendly {

        MAKE_GPU_FRIENDLY(SignumFunction)

        DevFloat operator()(DevFloat x) const override {
            return SIGN(x);
        }

        Function_ptr Clone() const override {
            return New<SignumFunction>();
        }
    };
}

#endif //V_SHAPE_SIGNUMFUNCTION_H
