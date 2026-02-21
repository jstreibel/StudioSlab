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
    struct FMetropolisSetup {
        using FNewValue = OutCategory;
        using FRandomSite = SiteType;

        using Temperature = DevFloat;
        using ΔSValue = DevFloat;

        using ΔSFunction          = std::function<ΔSValue(FRandomSite, FNewValue)>;
        using ModifyFunction      = std::function<void(FRandomSite, FNewValue)>;
        using SamplerFunction     = std::function<Vector<FRandomSite>()>;
        using ValueDrawerFunction = std::function<FNewValue(FRandomSite)>;
        using AcceptanceFunction  = std::function<bool(ΔSValue)>;

        using NewValue [[deprecated("Use FNewValue")]] = FNewValue;
        using RandomSite [[deprecated("Use FRandomSite")]] = FRandomSite;

        ΔSFunction          ΔS;
        ModifyFunction      modify;
        SamplerFunction     sample_locations;
        ValueDrawerFunction draw_value;
        AcceptanceFunction  should_accept;
    };

    template<typename SiteType, typename OutCategory>
    using MetropolisSetup [[deprecated("Use FMetropolisSetup")]] = FMetropolisSetup<SiteType, OutCategory>;
}

#endif //STUDIOSLAB_METROPOLIS_SETUP_H
