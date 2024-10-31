//
// Created by joao on 10/29/24.
//

#ifndef STUDIOSLAB_METROPOLIS_CONFIG_H
#define STUDIOSLAB_METROPOLIS_CONFIG_H

#include "Utils/Numbers.h"
#include "Math/Numerics/ODE/SimConfig/NumericConfig.h"

namespace Slab::Math {

    class MetropolisConfig : public NumericConfig {
        UInt max_steps;

    public:
        explicit MetropolisConfig(UInt max_steps);

        auto getn() const -> UInt override;

        auto to_string() const -> Str override;
    };

}

#endif //STUDIOSLAB_METROPOLIS_CONFIG_H
