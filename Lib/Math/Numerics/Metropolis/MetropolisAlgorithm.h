//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_METROPOLISALGORITHM_H
#define STUDIOSLAB_METROPOLISALGORITHM_H

#include "Utils/Numbers.h"
#include "Metropolis-Setup.h"

namespace Slab::Math {

    using Temperature = DevFloat;

    template<typename SiteType, typename NewValueType>
    class FMetropolisAlgorithm {
    public:
        using Setup = FMetropolisSetup<SiteType, NewValueType>;

        explicit FMetropolisAlgorithm(Setup setup) : algorithms(setup) { };

        void step() {
            for(IN site : algorithms.sample_locations()) {
                fix new_value = algorithms.draw_value(site);

                if(algorithms.should_accept(algorithms.ΔS(site, new_value)))
                    algorithms.modify(site, new_value);
            }
        }

    protected:
        Setup algorithms;

    };

    template<typename SiteType, typename NewValueType>
    using MetropolisAlgorithm [[deprecated("Use FMetropolisAlgorithm")]] = FMetropolisAlgorithm<SiteType, NewValueType>;



} // Slab::Math

#endif //STUDIOSLAB_METROPOLISALGORITHM_H
