//
// Created by joao on 11/09/23.
//

#include "ComplexMagnitude.h"

#include <utility>

namespace RtoR {
    ComplexMagnitude::ComplexMagnitude(RtoC::Function::Ptr func)
    : RtoCFunction(std::move(func)) {    }

    Real ComplexMagnitude::operator()(Real x) const {
        auto &func = *RtoCFunction;

        return abs(func(x));
    }


} // RtoR