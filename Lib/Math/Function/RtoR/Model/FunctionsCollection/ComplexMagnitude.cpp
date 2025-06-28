//
// Created by joao on 11/09/23.
//

#include "ComplexMagnitude.h"

#include <utility>

namespace Slab::Math::RtoR {
    ComplexMagnitude::ComplexMagnitude(RtoC::Function_ptr func)
    : RtoCFunction(std::move(func)) {    }

    DevFloat ComplexMagnitude::operator()(DevFloat x) const {
        auto &func = *RtoCFunction;

        return abs(func(x));
    }


} // RtoR