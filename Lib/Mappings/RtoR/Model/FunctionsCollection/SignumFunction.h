//
// Created by joao on 11/09/2019.
//

#ifndef V_SHAPE_SIGNUMFUNCTION_H
#define V_SHAPE_SIGNUMFUNCTION_H

#include "Mappings/RtoR/Model/RtoRFunction.h"

#include "Phys/Function/GPUFriendly.h"

namespace RtoR {
    class SignumFunction : public FunctionT, Base::GPUFriendly {

        MAKE_GPU_FRIENDLY(SignumFunction)

        Real operator()(Real x) const override {
            return SIGN(x);
        }

        FunctionT<Real, Real> *Clone() const override {
            return new SignumFunction;
        }
    };
}

#endif //V_SHAPE_SIGNUMFUNCTION_H
