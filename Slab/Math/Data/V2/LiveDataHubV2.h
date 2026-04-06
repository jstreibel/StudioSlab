#ifndef STUDIOSLAB_LIVE_DATA_HUB_V2_H
#define STUDIOSLAB_LIVE_DATA_HUB_V2_H

#include "LiveTopicsV2.h"

#include <map>
#include <mutex>

namespace Slab::Math::LiveData::V2 {

    class FSessionLiveViewV2;

    class FLiveDataHubV2 {
        mutable std::mutex Mutex;

        struct FSessionStreamEntry {
            TPointer<FSessionViewTopicV2> SessionTopic = nullptr;
            TPointer<FSessionTelemetryTopicV2> TelemetryTopic = nullptr;
            TPointer<FSessionSnapshotTopicV2> SnapshotTopic = nullptr;
            TPointer<FSessionStatusTopicV2> StatusTopic = nullptr;
            TPointer<FSessionLiveViewV2> LiveViewFacade = nullptr;
        };

        std::map<Str, FSessionStreamEntry> SessionStreams;

    public:
        [[nodiscard]] auto GetOrCreateSessionLiveView(const Str &topicName) -> TPointer<FSessionLiveViewV2>;
        [[nodiscard]] auto FindSessionLiveView(const Str &topicName) const -> TPointer<FSessionLiveViewV2>;
        [[nodiscard]] auto ListSessionLiveViewTopics() const -> StrVector;
    };

    DefinePointers(FLiveDataHubV2)

} // namespace Slab::Math::LiveData::V2

#endif // STUDIOSLAB_LIVE_DATA_HUB_V2_H
