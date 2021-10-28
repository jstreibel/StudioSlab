//
// Created by joao on 11/09/2019.
//

#ifndef V_SHAPE_SIGNUMFUNCTION_H
#define V_SHAPE_SIGNUMFUNCTION_H

#include "Studios/Util/Workaround/ABIDef.h"
#include "Studios/Math/Maps/RtoR/Model/RtoRFunction.h"
#include "Studios/Math/Core/GPUFriendly.h"

namespace RtoR {
    class SignumFunction : public Function, Base::GPUFriendly {

        MAKE_GPU_FRIENDLY(SignumFunction)

        Real operator()(Real x) const override {
            return SIGN(x);
        }

        Function<Real, Real> *Clone() const override {
            return new SignumFunction;
        }
    };
}

#endif //V_SHAPE_SIGNUMFUNCTION_H
