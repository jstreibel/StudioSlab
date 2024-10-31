//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_R2TOR_METROPOLIS_H
#define STUDIOSLAB_R2TOR_METROPOLIS_H

#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Math/Numerics/Metropolis/MetropolisAlgorithm.h"
#include "Math/Numerics/Metropolis/Metropolis-Setup.h"

namespace Slab::Math {


    class RtoRMetropolis : public MetropolisAlgorithm {
        using RtoRMetropolisSetup = MetropolisSetup<UInt, Real>;

        Pointer<RtoR::NumericFunction> function;
        RtoRMetropolisSetup algorithms;

    public:
        RtoRMetropolis(const Pointer<RtoR::NumericFunction_CPU>& function,
                        RtoRMetropolisSetup setup)
        : MetropolisAlgorithm()
        , function(function)
        , algorithms(std::move(setup))
        {        }

        void step() override {
            for(IN site : algorithms.sample_locations()) {
                fix new_value = algorithms.draw_value(site);

                if(algorithms.should_accept(algorithms.Δ_δSδϕ(site, new_value)))
                    algorithms.modify(site, new_value);
            }
        }
    };

} // Slab::Math

#endif //STUDIOSLAB_R2TOR_METROPOLIS_H
