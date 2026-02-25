#ifndef STUDIOSLAB_LIVE_TOPICS_V2_H
#define STUDIOSLAB_LIVE_TOPICS_V2_H

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

    enum class ESessionRunStateV2 {
        Running,
        Finished,
        Aborted
    };

    struct FSessionStatusV2 {
        ESessionRunStateV2 RunState = ESessionRunStateV2::Running;
        Numerics::V2::EEventReasonV2 LastReason = Numerics::V2::EEventReasonV2::Initial;
        UIntBig PublishedVersion = 0;
        bool bHasBoundSession = false;
        bool bTerminal = false;
    };

    enum class ELiveTopicKindV2 {
        SessionView,
        SessionTelemetry,
        SessionStatus
    };

    class FLiveTopicV2 {
        Str Name;
        ELiveTopicKindV2 Kind;

    public:
        explicit FLiveTopicV2(ELiveTopicKindV2 kind, Str name = "")
        : Name(std::move(name))
        , Kind(kind) {
        }

        virtual ~FLiveTopicV2() = default;

        [[nodiscard]] auto GetName() const -> Str { return Name; }
        [[nodiscard]] auto GetKind() const -> ELiveTopicKindV2 { return Kind; }
    };

    class FSessionTelemetryTopicV2 final : public FLiveTopicV2 {
        mutable std::mutex Mutex;
        std::optional<FSessionTelemetryV2> LastTelemetry = std::nullopt;

    public:
        explicit FSessionTelemetryTopicV2(Str name = "")
        : FLiveTopicV2(ELiveTopicKindV2::SessionTelemetry, std::move(name)) {
        }

        auto PublishEvent(const Numerics::V2::FSimulationEventV2 &event) -> void;
        [[nodiscard]] auto TryGetTelemetry() const -> std::optional<FSessionTelemetryV2>;
    };

    class FSessionViewTopicV2 final : public FLiveTopicV2 {
        mutable std::mutex Mutex;
        TVolatile<const Numerics::V2::FSimulationSessionV2> Session;

    public:
        explicit FSessionViewTopicV2(Str name = "")
        : FLiveTopicV2(ELiveTopicKindV2::SessionView, std::move(name)) {
        }

        auto BindSession(const TVolatile<const Numerics::V2::FSimulationSessionV2> &session) -> void;
        auto InvalidateSessionBinding() -> void;
        auto PublishEvent(const Numerics::V2::FSimulationEventV2 &event) -> void;

        [[nodiscard]] auto HasBoundSession() const -> bool;
        [[nodiscard]] auto AcquireReadLease() const -> std::optional<Numerics::V2::FSessionReadLeaseV2>;
        [[nodiscard]] auto TryAcquireReadLease() const -> std::optional<Numerics::V2::FSessionReadLeaseV2>;
    };

    class FSessionStatusTopicV2 final : public FLiveTopicV2 {
        mutable std::mutex Mutex;
        std::optional<FSessionStatusV2> LastStatus = std::nullopt;

    public:
        explicit FSessionStatusTopicV2(Str name = "")
        : FLiveTopicV2(ELiveTopicKindV2::SessionStatus, std::move(name)) {
        }

        auto PublishEvent(const Numerics::V2::FSimulationEventV2 &event) -> void;
        [[nodiscard]] auto TryGetStatus() const -> std::optional<FSessionStatusV2>;
    };

    DefinePointers(FLiveTopicV2)
    DefinePointers(FSessionTelemetryTopicV2)
    DefinePointers(FSessionViewTopicV2)
    DefinePointers(FSessionStatusTopicV2)

} // namespace Slab::Math::LiveData::V2

#endif // STUDIOSLAB_LIVE_TOPICS_V2_H
