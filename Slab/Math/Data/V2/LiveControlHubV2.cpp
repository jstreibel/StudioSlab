#include "LiveControlHubV2.h"

namespace Slab::Math::LiveControl::V2 {

    auto FLiveControlHubV2::GetOrCreateTopic(const Str &topicName) -> TPointer<FControlTopicV2> {
        std::lock_guard lock(Mutex);

        auto &topic = Topics[topicName];
        if (topic == nullptr) {
            topic = New<FControlTopicV2>(topicName);
        }

        return topic;
    }

    auto FLiveControlHubV2::FindTopic(const Str &topicName) const -> TPointer<FControlTopicV2> {
        std::lock_guard lock(Mutex);

        const auto it = Topics.find(topicName);
        if (it == Topics.end()) return nullptr;
        return it->second;
    }

    auto FLiveControlHubV2::ListTopics() const -> StrVector {
        std::lock_guard lock(Mutex);

        StrVector names;
        names.reserve(Topics.size());
        for (const auto &[name, topic] : Topics) {
            (void) topic;
            names.push_back(name);
        }
        return names;
    }

} // namespace Slab::Math::LiveControl::V2
