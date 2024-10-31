//
// Created by joao on 10/29/24.
//

#ifndef STUDIOSLAB_R2TOR_METROPOLIS_CONFIG_H
#define STUDIOSLAB_R2TOR_METROPOLIS_CONFIG_H

#include "Utils/Numbers.h"
#include "Math/Numerics/ODE/SimConfig/NumericConfig.h"

namespace Slab::Math {

    class MetropolisRtoRConfig : public NumericConfig {
        UInt max_steps;

    public:
        explicit MetropolisRtoRConfig(UInt max_steps);

        auto getn() const -> UInt override;

        auto to_string() const -> Str override;
    };

}

#endif //STUDIOSLAB_R2TOR_METROPOLIS_CONFIG_H
