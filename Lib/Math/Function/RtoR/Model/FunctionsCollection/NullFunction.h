//
// Created by joao on 13/09/2019.
//

#ifndef V_SHAPE_NULLFUNCTION_H
#define V_SHAPE_NULLFUNCTION_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {
    class NullFunction : public RtoR::Function {
        public:
            inline Real operator()(Real x) const override { return 0.0; }

            Function_ptr diff(int n) const override { if(n==0) return New <NullFunction> (); else throw Exception("No diff NullFunc n>0"); }
            Real diff(int n, Real x) const override { if(n==0) return 0.0; else throw "No diff NullFunc n>0"; }

            Function_ptr Clone() const override { return New <NullFunction> (); }

        Str symbol() const override {
            return "∅";
        }
    };
}

#endif //V_SHAPE_NULLFUNCTION_H
