#ifndef STUDIOSLAB_STATE_SNAPSHOT_LISTENER_V2_H
#define STUDIOSLAB_STATE_SNAPSHOT_LISTENER_V2_H

#include "ListenerV2.h"

#include <mutex>
#include <optional>

namespace Slab::Math::Numerics::V2 {

    struct FStateSnapshotEnvelopeV2 {
        Base::EquationState_ptr State = nullptr;
        FSimulationCursorV2 Cursor{};
        EEventReasonV2 Reason = EEventReasonV2::Scheduled;
        UIntBig PublishedVersion = 0;
    };

    class FStateSnapshotListenerV2 final : public IListenerV2 {
        mutable std::mutex SnapshotMutex;
        std::optional<FStateSnapshotEnvelopeV2> Snapshot = std::nullopt;
        Str Name;

        auto Capture(const FSimulationEventV2 &event) -> void;

    public:
        explicit FStateSnapshotListenerV2(Str name = "State Snapshot Listener V2");

        auto OnRunStarted(const FSimulationEventV2 &initialEvent) -> void override;
        auto OnSample(const FSimulationEventV2 &event) -> void override;
        auto OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool override;

        [[nodiscard]] auto GetName() const -> Str override;
        [[nodiscard]] auto TryGetSnapshot() const -> std::optional<FStateSnapshotEnvelopeV2>;
        [[nodiscard]] auto HasSnapshot() const -> bool;
        [[nodiscard]] auto GetSnapshot() const -> Base::EquationState_constptr;
        [[nodiscard]] auto GetMutableSnapshot() -> Base::EquationState_ptr;
        [[nodiscard]] auto GetSnapshotCursor() const -> FSimulationCursorV2;
        [[nodiscard]] auto GetSnapshotReason() const -> EEventReasonV2;
        [[nodiscard]] auto GetSnapshotVersion() const -> UIntBig;
    };

    DefinePointers(FStateSnapshotListenerV2)

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_STATE_SNAPSHOT_LISTENER_V2_H
