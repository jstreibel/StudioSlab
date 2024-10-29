//
// Created by joao on 10/28/24.
//

#include "Metropolis.h"
#include "Utils/RandUtils.h"
#include "Math/Thermal/ThermoUtils.h"

namespace Slab::Math {

    Metropolis::Metropolis(Temperature T) : T(T) {
    }

    bool Metropolis::should_accept(Real ΔE) const {
        if (ΔE < 0) return true;

        const double r = RandUtils::RandomUniformReal01();
        const double z = ThermoUtils::BoltzmannWeight(T, ΔE);

        return r < z;
    }


}