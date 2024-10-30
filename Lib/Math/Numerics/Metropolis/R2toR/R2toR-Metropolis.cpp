//
// Created by joao on 10/28/24.
//

#include "R2toR-Metropolis.h"

#include <utility>
#include "Utils/RandUtils.h"
#include "Math/Thermal/ThermoUtils.h"

namespace Slab::Math {
    R2toRMetropolis::R2toRMetropolis(const Pointer<R2toR::NumericFunction_CPU>& function,
                                     R2toRMetropolisSetup setup, Temperature T)
    : MetropolisAlgorithm(T)
    , function(function)
    , algorithms(std::move(setup))
    {
    }

    void R2toRMetropolis::step() {
        fix n = function->getN();
        fix m = function->getM();
        fix N = n*m;

        for(auto deltas=0; deltas<N; ++deltas) {
            fix site = algorithms.sample();
            fix new_value = algorithms.draw_value(site);

            if(algorithms.should_accept(algorithms.ΔS(site, new_value)))
                algorithms.modify(site, new_value);
        }
    }

} // Slab::Math