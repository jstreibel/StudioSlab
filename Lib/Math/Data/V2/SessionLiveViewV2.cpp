#include "SessionLiveViewV2.h"

namespace Slab::Math::LiveData::V2 {

    auto FSessionLiveViewV2::BindSession(const Numerics::V2::FSimulationSessionV2 *session) -> void {
        std::lock_guard lock(Mutex);

        if (Session != nullptr && session != nullptr && Session != session) {
            throw Exception("FSessionLiveViewV2 cannot be rebound to a different session.");
        }

        Session = session;
    }

    auto FSessionLiveViewV2::PublishEvent(const Numerics::V2::FSimulationEventV2 &event) -> void {
        std::lock_guard lock(Mutex);

        if (Session != nullptr && event.Session != nullptr && Session != event.Session) {
            throw Exception("FSessionLiveViewV2 received events from multiple sessions.");
        }

        if (event.Session != nullptr) Session = event.Session;

        LastTelemetry = FSessionTelemetryV2{
            .Cursor = event.Cursor,
            .LastReason = event.Reason,
            .PublishedVersion = event.PublishedVersion,
            .bHasState = (event.State != nullptr),
            .bRealtimeBestEffort = event.bIsRealtimeBestEffort
        };
    }

    auto FSessionLiveViewV2::HasBoundSession() const -> bool {
        std::lock_guard lock(Mutex);
        return Session != nullptr;
    }

    auto FSessionLiveViewV2::TryGetTelemetry() const -> std::optional<FSessionTelemetryV2> {
        std::lock_guard lock(Mutex);
        return LastTelemetry;
    }

    auto FSessionLiveViewV2::AcquireReadLease() const -> std::optional<Numerics::V2::FSessionReadLeaseV2> {
        const Numerics::V2::FSimulationSessionV2 *session = nullptr;
        {
            std::lock_guard lock(Mutex);
            session = Session;
        }

        if (session == nullptr) return std::nullopt;
        return session->AcquireReadLease();
    }

    auto FSessionLiveViewV2::TryAcquireReadLease() const -> std::optional<Numerics::V2::FSessionReadLeaseV2> {
        const Numerics::V2::FSimulationSessionV2 *session = nullptr;
        {
            std::lock_guard lock(Mutex);
            session = Session;
        }

        if (session == nullptr) return std::nullopt;
        return session->TryAcquireReadLease();
    }

} // namespace Slab::Math::LiveData::V2
