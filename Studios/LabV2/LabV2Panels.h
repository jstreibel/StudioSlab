#ifndef STUDIOSLAB_LAB_V2_PANELS_H
#define STUDIOSLAB_LAB_V2_PANELS_H

#include "Math/Data/V2/LiveControlHubV2.h"
#include "Math/Data/V2/LiveDataHubV2.h"

namespace Slab::Studios::LabV2::Panels {

    auto ShowTasksPanel(Str &nameFilter,
                        bool &bOnlyRunning,
                        bool &bHideSuccess,
                        bool &bOnlyNumeric) -> void;

    auto ShowLiveDataV2Panel(const TPointer<Math::LiveData::V2::FLiveDataHubV2> &hub,
                             Str &topicFilter,
                             bool &bOnlyBound,
                             Str &selectedTopic) -> void;

    auto ShowLiveControlV2Panel(const TPointer<Math::LiveControl::V2::FLiveControlHubV2> &hub,
                                Str &topicFilter,
                                bool &bLevelsOnly,
                                Str &selectedTopic) -> void;

    auto ShowKG2DControlSourcePanelAndPublish(
        const TPointer<Math::LiveControl::V2::FLiveControlHubV2> &hub,
        bool &bPublish,
        DevFloat &xCenter,
        DevFloat &yCenter,
        DevFloat &width,
        DevFloat &amplitude,
        bool &bEnabled,
        const Str &topicPrefix) -> void;

} // namespace Slab::Studios::LabV2::Panels

#endif
