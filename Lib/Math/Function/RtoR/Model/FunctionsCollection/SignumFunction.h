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

        Real operator()(Real x) const override {
            return SIGN(x);
        }

        RtoR::Function *Clone() const override {
            return new SignumFunction;
        }
    };
}

#endif //V_SHAPE_SIGNUMFUNCTION_H
