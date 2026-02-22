#include "SessionLiveViewPublisherListenerV2.h"

namespace Slab::Math::Numerics::V2 {

    FSessionLiveViewPublisherListenerV2::FSessionLiveViewPublisherListenerV2(
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
        Str name)
    : LiveView(liveView)
    , Name(std::move(name)) {
        if (LiveView == nullptr) {
            throw Exception("FSessionLiveViewPublisherListenerV2 requires a live view.");
        }
    }

    auto FSessionLiveViewPublisherListenerV2::Publish(const FSimulationEventV2 &event) const -> void {
        if (LiveView == nullptr) return;
        LiveView->PublishEvent(event);
    }

    auto FSessionLiveViewPublisherListenerV2::OnRunStarted(const FSimulationEventV2 &initialEvent) -> void {
        Publish(initialEvent);
    }

    auto FSessionLiveViewPublisherListenerV2::OnSample(const FSimulationEventV2 &event) -> void {
        Publish(event);
    }

    auto FSessionLiveViewPublisherListenerV2::OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool {
        Publish(finalEvent);
        return true;
    }

    auto FSessionLiveViewPublisherListenerV2::GetName() const -> Str {
        return Name;
    }

    auto FSessionLiveViewPublisherListenerV2::GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2> {
        return LiveView;
    }

} // namespace Slab::Math::Numerics::V2
