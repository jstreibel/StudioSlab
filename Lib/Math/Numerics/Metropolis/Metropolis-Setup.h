//
// Created by joao on 10/29/24.
//

#ifndef STUDIOSLAB_METROPOLIS_SETUP_H
#define STUDIOSLAB_METROPOLIS_SETUP_H

#include "Utils/Arrays.h"
#include "Utils/Numbers.h"
#include <functional>

namespace Slab::Math {
    template<typename SiteType, typename OutCategory>
    struct MetropolisSetup {
        using NewValue = OutCategory;
        using RandomSite = SiteType;

        using Temperature = Real;
        using ΔSValue = Real;

        using ΔSFunction          = std::function<ΔSValue(RandomSite, NewValue)>;
        using ModifyFunction      = std::function<void(RandomSite, NewValue)>;
        using SamplerFunction     = std::function<Vector<RandomSite>()>;
        using ValueDrawerFunction = std::function<NewValue(RandomSite)>;
        using AcceptanceFunction  = std::function<bool(ΔSValue)>;

        ΔSFunction          Δ_δSδϕ;
        ModifyFunction      modify;
        SamplerFunction     sample_locations;
        ValueDrawerFunction draw_value;
        AcceptanceFunction  should_accept;
    };
}

#endif //STUDIOSLAB_METROPOLIS_SETUP_H
