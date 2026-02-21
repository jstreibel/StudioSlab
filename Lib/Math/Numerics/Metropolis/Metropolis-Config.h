//
// Created by joao on 10/29/24.
//

#ifndef STUDIOSLAB_METROPOLIS_CONFIG_H
#define STUDIOSLAB_METROPOLIS_CONFIG_H

#include "Utils/Numbers.h"
#include "Math/Numerics/ODE/SimConfig/NumericConfig.h"

namespace Slab::Math {

    class FMetropolisConfig : public FNumericConfig {
        UInt max_steps;

    public:
        explicit FMetropolisConfig(UInt max_steps);

        auto Get_n() const -> UIntBig override;

        auto to_string() const -> Str override;
    };

    using MetropolisConfig [[deprecated("Use FMetropolisConfig")]] = FMetropolisConfig;

}

#endif //STUDIOSLAB_METROPOLIS_CONFIG_H
