#ifndef STUDIOSLAB_SESSION_LIVE_VIEW_V2_H
#define STUDIOSLAB_SESSION_LIVE_VIEW_V2_H

#include "Math/Numerics/V2/Events/SimulationEventV2.h"
#include "Math/Numerics/V2/Runtime/SimulationSessionV2.h"

#include <mutex>
#include <optional>

namespace Slab::Math::LiveData::V2 {

    struct FSessionTelemetryV2 {
        Numerics::V2::FSimulationCursorV2 Cursor;
        Numerics::V2::EEventReasonV2 LastReason = Numerics::V2::EEventReasonV2::Initial;
        UIntBig PublishedVersion = 0;
        bool bHasState = false;
        bool bRealtimeBestEffort = false;
    };

    class FSessionLiveViewV2 {
        mutable std::mutex Mutex;

        const Numerics::V2::FSimulationSessionV2 *Session = nullptr;
        std::optional<FSessionTelemetryV2> LastTelemetry = std::nullopt;

    public:
        auto BindSession(const Numerics::V2::FSimulationSessionV2 *session) -> void;
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
