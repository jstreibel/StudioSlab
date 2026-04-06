//
// Created by joao on 24/12/24.
//

#ifndef STUDIOSLAB_RTOR_METROPOLIS_CONFIG_H
#define STUDIOSLAB_RTOR_METROPOLIS_CONFIG_H

#include "Math/Numerics/ODE/SimConfig/NumericConfig.h"

namespace Slab::Math {
    // Don't touch:
#define DONT_REGISTER false
#define DO_REGISTER true


    /* ********** CONFIG **************/

    class FMetropolisRtoRConfig : public FNumericConfig {
        UInt max_steps;

    public:
        explicit FMetropolisRtoRConfig(UInt max_steps);

        auto Get_n() const -> UIntBig override;;

        auto to_string() const -> Str override;;
    };

    using MetropolisRtoRConfig [[deprecated("Use FMetropolisRtoRConfig")]] = FMetropolisRtoRConfig;

    /* ********************************/
}

#endif //STUDIOSLAB_RTOR_METROPOLIS_CONFIG_H
