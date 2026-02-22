#include "SessionLiveViewV2.h"

namespace Slab::Math::LiveData::V2 {

    auto FSessionLiveViewV2::BindSession(const TVolatile<const Numerics::V2::FSimulationSessionV2> &session) -> void {
        std::lock_guard lock(Mutex);

        const auto currentSession = Session.lock();
        const auto incomingSession = session.lock();

        if (currentSession != nullptr && incomingSession != nullptr && currentSession != incomingSession) {
            throw Exception("FSessionLiveViewV2 cannot be rebound to a different session.");
        }

        Session = session;
    }

    auto FSessionLiveViewV2::InvalidateSessionBinding() -> void {
        std::lock_guard lock(Mutex);
        Session = {};
    }

    auto FSessionLiveViewV2::PublishEvent(const Numerics::V2::FSimulationEventV2 &event) -> void {
        std::lock_guard lock(Mutex);

        const auto currentSession = Session.lock();
        const auto incomingSession = event.SessionRef.lock();

        if (currentSession != nullptr && incomingSession != nullptr && currentSession != incomingSession) {
            throw Exception("FSessionLiveViewV2 received events from multiple sessions.");
        }

        if (incomingSession != nullptr) Session = event.SessionRef;

        LastTelemetry = FSessionTelemetryV2{
            .Cursor = event.Cursor,
            .LastReason = event.Reason,
            .PublishedVersion = event.PublishedVersion,
            .bHasState = (event.State != nullptr),
            .bRealtimeBestEffort = event.bIsRealtimeBestEffort
        };

        if (event.Reason == Numerics::V2::EEventReasonV2::Final ||
            event.Reason == Numerics::V2::EEventReasonV2::AbortFinal) {
            Session = {};
        }
    }

    auto FSessionLiveViewV2::HasBoundSession() const -> bool {
        std::lock_guard lock(Mutex);
        return !Session.expired();
    }

    auto FSessionLiveViewV2::TryGetTelemetry() const -> std::optional<FSessionTelemetryV2> {
        std::lock_guard lock(Mutex);
        return LastTelemetry;
    }

    auto FSessionLiveViewV2::AcquireReadLease() const -> std::optional<Numerics::V2::FSessionReadLeaseV2> {
        TPointer<const Numerics::V2::FSimulationSessionV2> session = nullptr;
        {
            std::lock_guard lock(Mutex);
            session = Session.lock();
        }

        if (session == nullptr) return std::nullopt;
        return session->AcquireReadLease();
    }

    auto FSessionLiveViewV2::TryAcquireReadLease() const -> std::optional<Numerics::V2::FSessionReadLeaseV2> {
        TPointer<const Numerics::V2::FSimulationSessionV2> session = nullptr;
        {
            std::lock_guard lock(Mutex);
            session = Session.lock();
        }

        if (session == nullptr) return std::nullopt;
        return session->TryAcquireReadLease();
    }

} // namespace Slab::Math::LiveData::V2
