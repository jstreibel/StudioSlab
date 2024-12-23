//
// Created by joao on 11/4/21.
//

#include "Measure.h"

#include <utility>

namespace Slab::Math {

    Measure::Measure(RealVector linearValues) : linearValues(std::move(linearValues)) {

    }

    Real Measure::operator*() {
        auto v = 1.;
        for (auto lv: linearValues)
            v *= lv;

        return v;
    }
}