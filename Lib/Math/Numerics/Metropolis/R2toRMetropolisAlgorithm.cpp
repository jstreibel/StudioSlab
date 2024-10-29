//
// Created by joao on 10/28/24.
//

#include "R2toRMetropolisAlgorithm.h"

#include <utility>
#include "Utils/RandUtils.h"
#include "Math/Thermal/ThermoUtils.h"

namespace Slab::Math {
    MetropolisAlgorithmR2toR::MetropolisAlgorithmR2toR(const Pointer<R2toR::NumericFunction_CPU>& function,
                                                       MetropolisSetup setup, Temperature T)
    : Metropolis(T)
    , function(function)
    , algorithms(std::move(setup))
    {
    }

    void MetropolisAlgorithmR2toR::step() {
        fix n = function->getN();
        fix m = function->getM();
        fix N = n*m;

        for(auto deltas=0; deltas<N; ++deltas) {
            fix site = algorithms.sample();
            fix new_value = algorithms.draw_value(site);

            if(should_accept(algorithms.ΔE(site, new_value)))
                algorithms.modify(site, new_value);
        }
    }

} // Slab::Math