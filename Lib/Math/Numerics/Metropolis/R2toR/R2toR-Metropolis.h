//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_R2TOR_METROPOLIS_H
#define STUDIOSLAB_R2TOR_METROPOLIS_H

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Numerics/Metropolis/MetropolisAlgorithm.h"
#include "R2toR-Metropolis-Setup.h"

namespace Slab::Math {

    class R2toRMetropolis : public MetropolisAlgorithm {
        Pointer<R2toR::NumericFunction> function;
        R2toRMetropolisSetup algorithms;

    public:
        R2toRMetropolis(const Pointer<R2toR::NumericFunction_CPU>& function,
                        R2toRMetropolisSetup setup)
        : MetropolisAlgorithm()
        , function(function)
        , algorithms(std::move(setup))
        {        }

        void step() override {
            fix n = function->getN();
            fix m = function->getM();
            fix N = n*m;

            for(auto deltas=0; deltas<N; ++deltas) {
                fix site = algorithms.sample_location();
                fix new_value = algorithms.draw_value(site);

                if(algorithms.should_accept(algorithms.Δ_δSδϕ(site, new_value)))
                    algorithms.modify(site, new_value);
            }
        }
    };

} // Slab::Math

#endif //STUDIOSLAB_R2TOR_METROPOLIS_H
