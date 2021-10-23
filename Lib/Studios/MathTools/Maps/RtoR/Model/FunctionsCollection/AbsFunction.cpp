//
// Created by joao on 11/09/2019.
//

#include "AbsFunction.h"
#include "SignumFunction.h"

using namespace RtoR;

Real AbsFunction::operator()(Real x) const {
    return ABS(x);
}

Function *AbsFunction::Clone() const {
    return new AbsFunction;
}

Function::FunctionPtr AbsFunction::diff(int n) const {
    return Function<Real, Real>::FunctionPtr(new SignumFunction);
}
