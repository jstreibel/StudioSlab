#include "LiveDataHubV2.h"

#include "SessionLiveViewV2.h"

namespace Slab::Math::LiveData::V2 {

    auto FLiveDataHubV2::GetOrCreateSessionLiveView(const Str &topicName) -> TPointer<FSessionLiveViewV2> {
        std::lock_guard lock(Mutex);

        auto &entry = SessionStreams[topicName];
        if (entry.SessionTopic == nullptr) {
            entry.SessionTopic = New<FSessionViewTopicV2>(topicName + "/session");
        }
        if (entry.TelemetryTopic == nullptr) {
            entry.TelemetryTopic = New<FSessionTelemetryTopicV2>(topicName + "/telemetry");
        }
        if (entry.StatusTopic == nullptr) {
            entry.StatusTopic = New<FSessionStatusTopicV2>(topicName + "/status");
        }
        if (entry.LiveViewFacade == nullptr) {
            entry.LiveViewFacade = New<FSessionLiveViewV2>(entry.SessionTopic, entry.TelemetryTopic, entry.StatusTopic);
        }

        return entry.LiveViewFacade;
    }

    auto FLiveDataHubV2::FindSessionLiveView(const Str &topicName) const -> TPointer<FSessionLiveViewV2> {
        std::lock_guard lock(Mutex);
        const auto it = SessionStreams.find(topicName);
        if (it == SessionStreams.end()) return nullptr;
        return it->second.LiveViewFacade;
    }

    auto FLiveDataHubV2::ListSessionLiveViewTopics() const -> StrVector {
        std::lock_guard lock(Mutex);
        StrVector names;
        names.reserve(SessionStreams.size());
        for (const auto &[name, entry] : SessionStreams) {
            (void) entry;
            names.push_back(name);
        }
        return names;
    }

} // namespace Slab::Math::LiveData::V2
