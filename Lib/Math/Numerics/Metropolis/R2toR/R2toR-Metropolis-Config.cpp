//
// Created by joao on 10/29/24.
//

#include "R2toR-Metropolis-Config.h"

namespace Slab::Math {
    // Don't touch:
    #define DONT_REGISTER false
    #define DO_REGISTER true


    MetropolisR2toRConfig::MetropolisR2toRConfig(UInt max_steps)
            : NumericConfig(DONT_REGISTER), max_steps(max_steps) {
        // registerToManager();
    }

    UInt MetropolisR2toRConfig::getn() const {
        return max_steps;
    }

    Str MetropolisR2toRConfig::to_string() const {
        return {};
    }

}