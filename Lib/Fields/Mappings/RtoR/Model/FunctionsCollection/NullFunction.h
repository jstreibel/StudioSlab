//
// Created by joao on 13/09/2019.
//

#ifndef V_SHAPE_NULLFUNCTION_H
#define V_SHAPE_NULLFUNCTION_H

#include "../RtoRFunction.h"

namespace RtoR {
    class NullFunction : public Function {
    public:
        Real operator()(Real x) const override { return 0.0; }

        Pointer diff(int n) const override { return Pointer(new NullFunction); }

        Function<Real, Real> *Clone() const override { return new NullFunction; }
    };
}

#endif //V_SHAPE_NULLFUNCTION_H
