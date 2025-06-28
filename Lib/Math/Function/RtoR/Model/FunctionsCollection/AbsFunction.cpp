//
// Created by joao on 11/09/2019.
//

#include "AbsFunction.h"
#include "SignumFunction.h"

namespace Slab::Math::RtoR {

    DevFloat AbsFunction::operator()(DevFloat x) const {
        return Abs(x);
    }

    Pointer<Function> AbsFunction::Clone() const {
        return New<AbsFunction>();
    }

    Pointer<Function> AbsFunction::diff(int n) const {
        return New<SignumFunction>();
    }

    Str AbsFunction::symbol() const {
        return "|ϕ|";
    }


}