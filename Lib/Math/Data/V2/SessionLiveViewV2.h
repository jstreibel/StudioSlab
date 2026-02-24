#ifndef STUDIOSLAB_SESSION_LIVE_VIEW_V2_H
#define STUDIOSLAB_SESSION_LIVE_VIEW_V2_H

#include "LiveTopicsV2.h"

namespace Slab::Math::LiveData::V2 {

    class FSessionLiveViewV2 {
        TPointer<FSessionViewTopicV2> SessionTopic;
        TPointer<FSessionTelemetryTopicV2> TelemetryTopic;

    public:
        FSessionLiveViewV2();
        FSessionLiveViewV2(TPointer<FSessionViewTopicV2> sessionTopic,
                           TPointer<FSessionTelemetryTopicV2> telemetryTopic);

        auto BindSession(const TVolatile<const Numerics::V2::FSimulationSessionV2> &session) -> void;
        auto InvalidateSessionBinding() -> void;

        auto PublishEvent(const Numerics::V2::FSimulationEventV2 &event) -> void;

        [[nodiscard]] auto HasBoundSession() const -> bool;
        [[nodiscard]] auto TryGetTelemetry() const -> std::optional<FSessionTelemetryV2>;

        [[nodiscard]] auto AcquireReadLease() const -> std::optional<Numerics::V2::FSessionReadLeaseV2>;
        [[nodiscard]] auto TryAcquireReadLease() const -> std::optional<Numerics::V2::FSessionReadLeaseV2>;
    };

    DefinePointers(FSessionLiveViewV2)

} // namespace Slab::Math::LiveData::V2

#endif // STUDIOSLAB_SESSION_LIVE_VIEW_V2_H
