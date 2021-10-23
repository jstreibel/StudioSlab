//
// Created by joao on 13/09/2019.
//

#ifndef V_SHAPE_NULLFUNCTION_H
#define V_SHAPE_NULLFUNCTION_H

#include "Studios/MathTools/Maps/RtoR/Model/RtoRFunction.h"

namespace RtoR {
    class NullFunction : public Function {
    public:
        Real operator()(Real x) const override { return 0.0; }

        FunctionPtr diff(int n) const override { return FunctionPtr(new NullFunction); }

        Function<Real, Real> *Clone() const override { return new NullFunction; }
    };
}

#endif //V_SHAPE_NULLFUNCTION_H