#include "StateSnapshotListenerV2.h"

namespace Slab::Math::Numerics::V2 {

    FStateSnapshotListenerV2::FStateSnapshotListenerV2(Str name)
    : Name(std::move(name)) {
    }

    auto FStateSnapshotListenerV2::Capture(const FSimulationEventV2 &event) -> void {
        if (event.State == nullptr) return;

        if (Snapshot == nullptr) {
            Snapshot = event.State->Replicate(std::nullopt);
        }
        if (Snapshot == nullptr) return;

        Snapshot->setData(*event.State);
        SnapshotCursor = event.Cursor;
        SnapshotReason = event.Reason;
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

    auto FStateSnapshotListenerV2::HasSnapshot() const -> bool {
        return Snapshot != nullptr;
    }

    auto FStateSnapshotListenerV2::GetSnapshot() const -> Base::EquationState_constptr {
        return Snapshot;
    }

    auto FStateSnapshotListenerV2::GetMutableSnapshot() -> Base::EquationState_ptr {
        return Snapshot;
    }

    auto FStateSnapshotListenerV2::GetSnapshotCursor() const -> const FSimulationCursorV2 & {
        return SnapshotCursor;
    }

    auto FStateSnapshotListenerV2::GetSnapshotReason() const -> EEventReasonV2 {
        return SnapshotReason;
    }

} // namespace Slab::Math::Numerics::V2
