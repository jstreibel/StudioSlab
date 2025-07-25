//
// Created by joao on 10/7/24.
//

#include "Diff.h"

#include <utility>

namespace Slab::Math::RtoR {
    Diff::Diff(TPointer<const RtoR::Function> base_function, DevFloat dx)
    : base_function(std::move(base_function)), dx(dx) {

    }

    DevFloat Diff::operator()(DevFloat x) const {
        fix &f = *base_function;
        return (.5/dx) * (f(x+dx)-f(x-dx));
    }

} // Slab::Math::RtoR