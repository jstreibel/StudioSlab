//
// Created by joao on 24/12/24.
//

#include "RtoR-Metropolis-Config.h"

namespace Slab::Math {

    MetropolisRtoRConfig::MetropolisRtoRConfig(UInt max_steps)
            : FNumericConfig(DONT_REGISTER), max_steps(max_steps) {
        // registerToManager();
    }

    auto MetropolisRtoRConfig::Get_n() const -> UInt { return max_steps; }

    auto MetropolisRtoRConfig::to_string() const -> Str { return {}; }
}