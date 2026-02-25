#include "SessionLiveViewV2.h"

namespace Slab::Math::LiveData::V2 {

    FSessionLiveViewV2::FSessionLiveViewV2()
    : SessionTopic(New<FSessionViewTopicV2>())
    , TelemetryTopic(New<FSessionTelemetryTopicV2>()) {
        StatusTopic = New<FSessionStatusTopicV2>();
    }

    FSessionLiveViewV2::FSessionLiveViewV2(TPointer<FSessionViewTopicV2> sessionTopic,
                                           TPointer<FSessionTelemetryTopicV2> telemetryTopic)
    : FSessionLiveViewV2(std::move(sessionTopic), std::move(telemetryTopic), nullptr) {
    }

    FSessionLiveViewV2::FSessionLiveViewV2(TPointer<FSessionViewTopicV2> sessionTopic,
                                           TPointer<FSessionTelemetryTopicV2> telemetryTopic,
                                           TPointer<FSessionStatusTopicV2> statusTopic)
    : SessionTopic(sessionTopic != nullptr ? std::move(sessionTopic) : New<FSessionViewTopicV2>())
    , TelemetryTopic(telemetryTopic != nullptr ? std::move(telemetryTopic) : New<FSessionTelemetryTopicV2>())
    , StatusTopic(statusTopic != nullptr ? std::move(statusTopic) : New<FSessionStatusTopicV2>()) {
    }

    auto FSessionLiveViewV2::BindSession(const TVolatile<const Numerics::V2::FSimulationSessionV2> &session) -> void {
        if (SessionTopic == nullptr) throw Exception("FSessionLiveViewV2 session topic is null.");
        SessionTopic->BindSession(session);
    }

    auto FSessionLiveViewV2::InvalidateSessionBinding() -> void {
        if (SessionTopic == nullptr) return;
        SessionTopic->InvalidateSessionBinding();
    }

    auto FSessionLiveViewV2::PublishEvent(const Numerics::V2::FSimulationEventV2 &event) -> void {
        if (SessionTopic != nullptr) SessionTopic->PublishEvent(event);
        if (TelemetryTopic != nullptr) TelemetryTopic->PublishEvent(event);
        if (StatusTopic != nullptr) StatusTopic->PublishEvent(event);
    }

    auto FSessionLiveViewV2::HasBoundSession() const -> bool {
        if (SessionTopic == nullptr) return false;
        return SessionTopic->HasBoundSession();
    }

    auto FSessionLiveViewV2::TryGetTelemetry() const -> std::optional<FSessionTelemetryV2> {
        if (TelemetryTopic == nullptr) return std::nullopt;
        return TelemetryTopic->TryGetTelemetry();
    }

    auto FSessionLiveViewV2::TryGetStatus() const -> std::optional<FSessionStatusV2> {
        if (StatusTopic == nullptr) return std::nullopt;
        return StatusTopic->TryGetStatus();
    }

    auto FSessionLiveViewV2::AcquireReadLease() const -> std::optional<Numerics::V2::FSessionReadLeaseV2> {
        if (SessionTopic == nullptr) return std::nullopt;
        return SessionTopic->AcquireReadLease();
    }

    auto FSessionLiveViewV2::TryAcquireReadLease() const -> std::optional<Numerics::V2::FSessionReadLeaseV2> {
        if (SessionTopic == nullptr) return std::nullopt;
        return SessionTopic->TryAcquireReadLease();
    }

} // namespace Slab::Math::LiveData::V2
