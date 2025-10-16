//
// Created by joao on 10/29/24.
//

#include "Metropolis-Config.h"

namespace Slab::Math {
    // Don't touch:
    #define DONT_REGISTER false
    #define DO_REGISTER true


    MetropolisConfig::MetropolisConfig(UInt max_steps)
            : FNumericConfig(DONT_REGISTER), max_steps(max_steps) {
    }

    UIntBig MetropolisConfig::Get_n() const {
        return max_steps;
    }

    Str MetropolisConfig::to_string() const {
        return {};
    }

}