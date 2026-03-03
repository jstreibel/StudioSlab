#ifndef STUDIOS_COMMON_SIMULATIONS_V2_KGR2TOR_CONTROL_TOPICS_V2_H
#define STUDIOS_COMMON_SIMULATIONS_V2_KGR2TOR_CONTROL_TOPICS_V2_H

#include "Utils/Types.h"

namespace Slab::Studios::Common::Simulations::V2 {

    inline constexpr const char *KG2DControlTopicPrefixDefaultV2 = "lab/control/kg2d";

    inline constexpr const char *KG2DForcingCenterSuffixV2 = "forcing/center";
    inline constexpr const char *KG2DForcingAmplitudeSuffixV2 = "forcing/amplitude";
    inline constexpr const char *KG2DForcingWidthSuffixV2 = "forcing/width";
    inline constexpr const char *KG2DForcingEnabledSuffixV2 = "forcing/enabled";

    struct FKG2DForcingTopicNamesV2 {
        Str Center;
        Str Amplitude;
        Str Width;
        Str Enabled;
    };

    [[nodiscard]] inline auto BuildKG2DForcingTopicNamesV2(const Str &topicPrefix)
        -> FKG2DForcingTopicNamesV2 {
        return {
            topicPrefix + "/" + KG2DForcingCenterSuffixV2,
            topicPrefix + "/" + KG2DForcingAmplitudeSuffixV2,
            topicPrefix + "/" + KG2DForcingWidthSuffixV2,
            topicPrefix + "/" + KG2DForcingEnabledSuffixV2
        };
    }

} // namespace Slab::Studios::Common::Simulations::V2

#endif
