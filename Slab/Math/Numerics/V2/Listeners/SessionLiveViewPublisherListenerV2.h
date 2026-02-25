#ifndef STUDIOSLAB_SESSION_LIVE_VIEW_PUBLISHER_LISTENER_V2_H
#define STUDIOSLAB_SESSION_LIVE_VIEW_PUBLISHER_LISTENER_V2_H

#include "ListenerV2.h"
#include "Math/Data/V2/SessionLiveViewV2.h"

namespace Slab::Math::Numerics::V2 {

    class FSessionLiveViewPublisherListenerV2 final : public IListenerV2 {
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView;
        Str Name;

        auto Publish(const FSimulationEventV2 &event) const -> void;

    public:
        explicit FSessionLiveViewPublisherListenerV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            Str name = "Session Live View Publisher V2");

        auto OnRunStarted(const FSimulationEventV2 &initialEvent) -> void override;
        auto OnSample(const FSimulationEventV2 &event) -> void override;
        auto OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool override;

        [[nodiscard]] auto GetName() const -> Str override;
        [[nodiscard]] auto GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2>;
    };

    DefinePointers(FSessionLiveViewPublisherListenerV2)

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_SESSION_LIVE_VIEW_PUBLISHER_LISTENER_V2_H
