//
// Created by joao on 13/09/2019.
//

#ifndef V_SHAPE_NULLFUNCTION_H
#define V_SHAPE_NULLFUNCTION_H

#include "Mappings/RtoR/Model/RtoRFunction.h"

namespace RtoR {
    class NullFunction : public Function {
    public:
        Real operator()(Real x) const override { return 0.0; }

        Ptr diff(int n) const override { if(n==0) return Ptr(new NullFunction); else throw "No diff NullFunc n>0"; }
        Real diff(int n, Real x) const override { if(n==0) return 0.0; else throw "No diff NullFunc n>0"; }

        Function<Real, Real> *Clone() const override { return new NullFunction; }
    };
}

#endif //V_SHAPE_NULLFUNCTION_H
