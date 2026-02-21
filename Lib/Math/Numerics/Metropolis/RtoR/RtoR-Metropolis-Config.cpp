//
// Created by joao on 24/12/24.
//

#include "RtoR-Metropolis-Config.h"

namespace Slab::Math {

    FMetropolisRtoRConfig::FMetropolisRtoRConfig(UInt max_steps)
            : FNumericConfig(DONT_REGISTER), max_steps(max_steps) {
        // registerToManager();
    }

    auto FMetropolisRtoRConfig::Get_n() const -> UIntBig { return max_steps; }

    auto FMetropolisRtoRConfig::to_string() const -> Str { return {}; }
}
