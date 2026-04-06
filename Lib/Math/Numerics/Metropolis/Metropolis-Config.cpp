//
// Created by joao on 10/29/24.
//

#include "Metropolis-Config.h"

namespace Slab::Math {
    // Don't touch:
    #define DONT_REGISTER false
    #define DO_REGISTER true


    FMetropolisConfig::FMetropolisConfig(UInt max_steps)
            : FNumericConfig(DONT_REGISTER), max_steps(max_steps) {
    }

    UIntBig FMetropolisConfig::Get_n() const {
        return max_steps;
    }

    Str FMetropolisConfig::to_string() const {
        return {};
    }

}
