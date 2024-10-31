//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_R2TOR_METROPOLIS_H
#define STUDIOSLAB_R2TOR_METROPOLIS_H

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Numerics/Metropolis/MetropolisAlgorithm.h"
#include "Math/Numerics/Metropolis/Metropolis-Setup.h"

namespace Slab::Math {

    struct RandomSite{ UInt i; UInt j; };
    using NewValue = Real;
    using R2toRMetropolisSetup = MetropolisSetup<RandomSite, NewValue>;

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
            for(IN site : algorithms.sample_locations()) {
                fix new_value = algorithms.draw_value(site);

                if(algorithms.should_accept(algorithms.Δ_δSδϕ(site, new_value)))
                    algorithms.modify(site, new_value);
            }
        }
    };

} // Slab::Math

#endif //STUDIOSLAB_R2TOR_METROPOLIS_H
