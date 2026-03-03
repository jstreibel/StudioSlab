#include "LiveTopicsV2.h"

namespace Slab::Math::LiveData::V2 {
    namespace {
        [[nodiscard]] auto IsTerminalEventReason(const Numerics::V2::EEventReasonV2 reason) -> bool {
            return reason == Numerics::V2::EEventReasonV2::Final
                || reason == Numerics::V2::EEventReasonV2::AbortFinal;
        }
    } // namespace


    auto FSessionTelemetryTopicV2::PublishEvent(const Numerics::V2::FSimulationEventV2 &event) -> void {
        std::lock_guard lock(Mutex);

        LastTelemetry = FSessionTelemetryV2{
            .Cursor = event.Cursor,
            .LastReason = event.Reason,
            .PublishedVersion = event.PublishedVersion,
            .bHasState = (event.State != nullptr),
            .bRealtimeBestEffort = event.bIsRealtimeBestEffort
        };
    }

    auto FSessionTelemetryTopicV2::TryGetTelemetry() const -> std::optional<FSessionTelemetryV2> {
        std::lock_guard lock(Mutex);
        return LastTelemetry;
    }

    auto FSessionViewTopicV2::BindSession(const TVolatile<const Numerics::V2::FSimulationSessionV2> &session) -> void {
        std::lock_guard lock(Mutex);

        const auto currentSession = Session.lock();
        const auto incomingSession = session.lock();

        if (currentSession != nullptr && incomingSession != nullptr && currentSession != incomingSession) {
            throw Exception("FSessionViewTopicV2 cannot be rebound to a different session.");
        }

        Session = session;
    }

    auto FSessionViewTopicV2::InvalidateSessionBinding() -> void {
        std::lock_guard lock(Mutex);
        Session = {};
    }

    auto FSessionViewTopicV2::PublishEvent(const Numerics::V2::FSimulationEventV2 &event) -> void {
        std::lock_guard lock(Mutex);

        const auto currentSession = Session.lock();
        const auto incomingSession = event.SessionRef.lock();

        if (currentSession != nullptr && incomingSession != nullptr && currentSession != incomingSession) {
            throw Exception("FSessionViewTopicV2 received events from multiple sessions.");
        }

        if (incomingSession != nullptr) Session = event.SessionRef;

        if (event.Reason == Numerics::V2::EEventReasonV2::Final ||
            event.Reason == Numerics::V2::EEventReasonV2::AbortFinal) {
            Session = {};
        }
    }

    auto FSessionViewTopicV2::HasBoundSession() const -> bool {
        std::lock_guard lock(Mutex);
        return !Session.expired();
    }

    auto FSessionViewTopicV2::AcquireReadLease() const -> std::optional<Numerics::V2::FSessionReadLeaseV2> {
        TPointer<const Numerics::V2::FSimulationSessionV2> session = nullptr;
        {
            std::lock_guard lock(Mutex);
            session = Session.lock();
        }

        if (session == nullptr) return std::nullopt;
        return session->AcquireReadLease();
    }

    auto FSessionViewTopicV2::TryAcquireReadLease() const -> std::optional<Numerics::V2::FSessionReadLeaseV2> {
        TPointer<const Numerics::V2::FSimulationSessionV2> session = nullptr;
        {
            std::lock_guard lock(Mutex);
            session = Session.lock();
        }

        if (session == nullptr) return std::nullopt;
        return session->TryAcquireReadLease();
    }

    auto FSessionStatusTopicV2::PublishEvent(const Numerics::V2::FSimulationEventV2 &event) -> void {
        std::lock_guard lock(Mutex);

        FSessionStatusV2 status;
        status.LastReason = event.Reason;
        status.PublishedVersion = event.PublishedVersion;

        switch (event.Reason) {
        case Numerics::V2::EEventReasonV2::Final:
            status.RunState = ESessionRunStateV2::Finished;
            status.bTerminal = true;
            break;
        case Numerics::V2::EEventReasonV2::AbortFinal:
            status.RunState = ESessionRunStateV2::Aborted;
            status.bTerminal = true;
            break;
        case Numerics::V2::EEventReasonV2::Initial:
        case Numerics::V2::EEventReasonV2::Scheduled:
        case Numerics::V2::EEventReasonV2::Forced:
            status.RunState = ESessionRunStateV2::Running;
            status.bTerminal = false;
            break;
        }

        status.bHasBoundSession = !event.SessionRef.expired() && !status.bTerminal;
        LastStatus = status;
    }

    auto FSessionStatusTopicV2::TryGetStatus() const -> std::optional<FSessionStatusV2> {
        std::lock_guard lock(Mutex);
        return LastStatus;
    }

    auto FSessionSnapshotTopicV2::PublishEvent(const Numerics::V2::FSimulationEventV2 &event) -> void {
        bool bShouldCaptureSnapshot = false;
        {
            std::lock_guard lock(Mutex);
            bShouldCaptureSnapshot = ActiveConsumerCount > 0 || IsTerminalEventReason(event.Reason);
        }
        if (!bShouldCaptureSnapshot) return;

        const auto sourceState = event.State;
        if (sourceState == nullptr) return;

        auto stateCopy = sourceState->Replicate(std::nullopt);
        if (stateCopy == nullptr) return;
        stateCopy->setData(*sourceState);

        FSessionSnapshotV2 snapshot;
        snapshot.State = std::move(stateCopy);
        snapshot.Cursor = event.Cursor;
        snapshot.LastReason = event.Reason;
        snapshot.PublishedVersion = event.PublishedVersion;

        std::lock_guard lock(Mutex);
        LastSnapshot = std::move(snapshot);
    }

    auto FSessionSnapshotTopicV2::TryGetSnapshot() const -> std::optional<FSessionSnapshotV2> {
        std::lock_guard lock(Mutex);
        return LastSnapshot;
    }

    auto FSessionSnapshotTopicV2::RegisterConsumer() -> void {
        std::lock_guard lock(Mutex);
        ++ActiveConsumerCount;
    }

    auto FSessionSnapshotTopicV2::UnregisterConsumer() -> void {
        std::lock_guard lock(Mutex);
        if (ActiveConsumerCount > 0) --ActiveConsumerCount;
    }

    auto FSessionSnapshotTopicV2::HasConsumers() const -> bool {
        std::lock_guard lock(Mutex);
        return ActiveConsumerCount > 0;
    }

} // namespace Slab::Math::LiveData::V2
