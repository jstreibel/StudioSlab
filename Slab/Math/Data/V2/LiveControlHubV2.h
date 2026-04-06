#ifndef STUDIOSLAB_LIVE_CONTROL_HUB_V2_H
#define STUDIOSLAB_LIVE_CONTROL_HUB_V2_H

#include "LiveControlTopicsV2.h"

#include <map>
#include <mutex>

namespace Slab::Math::LiveControl::V2 {

    class FLiveControlHubV2 {
        mutable std::mutex Mutex;
        std::map<Str, TPointer<FControlTopicV2>> Topics;

    public:
        [[nodiscard]] auto GetOrCreateTopic(const Str &topicName) -> TPointer<FControlTopicV2>;
        [[nodiscard]] auto FindTopic(const Str &topicName) const -> TPointer<FControlTopicV2>;
        [[nodiscard]] auto ListTopics() const -> StrVector;
    };

    DefinePointers(FLiveControlHubV2)

} // namespace Slab::Math::LiveControl::V2

#endif // STUDIOSLAB_LIVE_CONTROL_HUB_V2_H
