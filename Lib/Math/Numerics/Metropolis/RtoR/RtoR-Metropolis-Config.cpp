//
// Created by joao on 10/29/24.
//

#include "RtoR-Metropolis-Config.h"

namespace Slab::Math {
    // Don't touch:
    #define DONT_REGISTER false
    #define DO_REGISTER true


    MetropolisRtoRConfig::MetropolisRtoRConfig(UInt max_steps)
            : NumericConfig(DONT_REGISTER), max_steps(max_steps) {
        // registerToManager();
    }

    UInt MetropolisRtoRConfig::getn() const {
        return max_steps;
    }

    Str MetropolisRtoRConfig::to_string() const {
        return {};
    }

}