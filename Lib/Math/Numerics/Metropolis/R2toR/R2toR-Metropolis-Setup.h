//
// Created by joao on 10/29/24.
//

#ifndef STUDIOSLAB_R2TOR_METROPOLIS_SETUP_H
#define STUDIOSLAB_R2TOR_METROPOLIS_SETUP_H

#include "Utils/Numbers.h"
#include <functional>

namespace Slab::Math {
    using RandomSite = struct { UInt i; UInt j; };

    using Temperature = Real;
    using NewValue = Real;
    using ΔSValue = Real;

    using ΔSFunction          = std::function<ΔSValue(RandomSite, NewValue)>;
    using ModifyFunction      = std::function<void(RandomSite, NewValue)>;
    using SamplerFunction     = std::function<RandomSite()>;
    using ValueDrawerFunction = std::function<NewValue(RandomSite)>;
    using AcceptanceFunction  = std::function<bool(ΔSValue)>;

    struct R2toRMetropolisSetup {
        ΔSFunction Δ_δSδϕ;
        ModifyFunction modify;
        SamplerFunction sample_location;
        ValueDrawerFunction draw_value;
        AcceptanceFunction should_accept;
    };
}

#endif //STUDIOSLAB_R2TOR_METROPOLIS_SETUP_H
