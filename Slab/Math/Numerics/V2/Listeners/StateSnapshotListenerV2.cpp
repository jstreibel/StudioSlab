#include "StateSnapshotListenerV2.h"

#include "Math/Numerics/V2/Runtime/SimulationSessionV2.h"

namespace Slab::Math::Numerics::V2 {

    FStateSnapshotListenerV2::FStateSnapshotListenerV2(Str name)
    : Name(std::move(name)) {
    }

    auto FStateSnapshotListenerV2::Capture(const FSimulationEventV2 &event) -> void {
        if (const auto session = event.SessionRef.lock(); session != nullptr) {
            const auto lease = session->AcquireReadLease();
            const auto sourceState = lease.GetState();
            if (sourceState == nullptr) return;

            auto stateCopy = sourceState->Replicate(std::nullopt);
            if (stateCopy == nullptr) return;
            stateCopy->setData(*sourceState);

            FStateSnapshotEnvelopeV2 nextSnapshot;
            nextSnapshot.State = std::move(stateCopy);
            nextSnapshot.Cursor = lease.GetCursor();
            nextSnapshot.Reason = event.Reason;
            nextSnapshot.PublishedVersion = lease.GetPublishedVersion();

            std::lock_guard lock(SnapshotMutex);
            Snapshot = std::move(nextSnapshot);
            return;
        }

        const auto sourceState = event.State;
        if (sourceState == nullptr) return;

        auto stateCopy = sourceState->Replicate(std::nullopt);
        if (stateCopy == nullptr) return;
        stateCopy->setData(*sourceState);

        FStateSnapshotEnvelopeV2 nextSnapshot;
        nextSnapshot.State = std::move(stateCopy);
        nextSnapshot.Cursor = event.Cursor;
        nextSnapshot.Reason = event.Reason;
        nextSnapshot.PublishedVersion = event.PublishedVersion;

        std::lock_guard lock(SnapshotMutex);
        Snapshot = std::move(nextSnapshot);
    }

    auto FStateSnapshotListenerV2::OnRunStarted(const FSimulationEventV2 &initialEvent) -> void {
        Capture(initialEvent);
    }

    auto FStateSnapshotListenerV2::OnSample(const FSimulationEventV2 &event) -> void {
        Capture(event);
    }

    auto FStateSnapshotListenerV2::OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool {
        Capture(finalEvent);
        return true;
    }

    auto FStateSnapshotListenerV2::GetName() const -> Str {
        return Name;
    }

    auto FStateSnapshotListenerV2::TryGetSnapshot() const -> std::optional<FStateSnapshotEnvelopeV2> {
        std::lock_guard lock(SnapshotMutex);
        return Snapshot;
    }

    auto FStateSnapshotListenerV2::HasSnapshot() const -> bool {
        std::lock_guard lock(SnapshotMutex);
        return Snapshot.has_value() && Snapshot->State != nullptr;
    }

    auto FStateSnapshotListenerV2::GetSnapshot() const -> Base::EquationState_constptr {
        std::lock_guard lock(SnapshotMutex);
        if (!Snapshot.has_value()) return nullptr;
        return Snapshot->State;
    }

    auto FStateSnapshotListenerV2::GetMutableSnapshot() -> Base::EquationState_ptr {
        std::lock_guard lock(SnapshotMutex);
        if (!Snapshot.has_value()) return nullptr;
        return Snapshot->State;
    }

    auto FStateSnapshotListenerV2::GetSnapshotCursor() const -> FSimulationCursorV2 {
        std::lock_guard lock(SnapshotMutex);
        if (!Snapshot.has_value()) return {};
        return Snapshot->Cursor;
    }

    auto FStateSnapshotListenerV2::GetSnapshotReason() const -> EEventReasonV2 {
        std::lock_guard lock(SnapshotMutex);
        if (!Snapshot.has_value()) return EEventReasonV2::Scheduled;
        return Snapshot->Reason;
    }

    auto FStateSnapshotListenerV2::GetSnapshotVersion() const -> UIntBig {
        std::lock_guard lock(SnapshotMutex);
        if (!Snapshot.has_value()) return 0;
        return Snapshot->PublishedVersion;
    }

} // namespace Slab::Math::Numerics::V2
