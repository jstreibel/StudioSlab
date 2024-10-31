//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_METROPOLISALGORITHM_H
#define STUDIOSLAB_METROPOLISALGORITHM_H

#include "Utils/Numbers.h"
#include "Metropolis-Setup.h"

namespace Slab::Math {

    using Temperature = Real;

    template<typename SiteType, typename NewValueType>
    class MetropolisAlgorithm {
    public:
        using Setup = MetropolisSetup<SiteType, NewValueType>;

        explicit MetropolisAlgorithm(Setup setup) : algorithms(setup) { };

        void step() {
            for(IN site : algorithms.sample_locations()) {
                fix new_value = algorithms.draw_value(site);

                if(algorithms.should_accept(algorithms.Δ_δSδϕ(site, new_value)))
                    algorithms.modify(site, new_value);
            }
        }

    protected:
        Setup algorithms;

    };



} // Slab::Math

#endif //STUDIOSLAB_METROPOLISALGORITHM_H
