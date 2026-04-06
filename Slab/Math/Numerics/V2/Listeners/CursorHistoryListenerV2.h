#ifndef STUDIOSLAB_CURSOR_HISTORY_LISTENER_V2_H
#define STUDIOSLAB_CURSOR_HISTORY_LISTENER_V2_H

#include "ListenerV2.h"

namespace Slab::Math::Numerics::V2 {

    struct FCursorHistorySampleV2 {
        FSimulationCursorV2 Cursor;
        EEventReasonV2 Reason = EEventReasonV2::Scheduled;
    };

    class FCursorHistoryListenerV2 final : public IListenerV2 {
        Vector<FCursorHistorySampleV2> Samples;
        Str Name;

        auto Store(const FSimulationEventV2 &event) -> void;

    public:
        explicit FCursorHistoryListenerV2(Str name = "Cursor History Listener V2");

        auto OnRunStarted(const FSimulationEventV2 &initialEvent) -> void override;
        auto OnSample(const FSimulationEventV2 &event) -> void override;
        auto OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool override;

        [[nodiscard]] auto GetName() const -> Str override;
        [[nodiscard]] auto GetSamples() const -> const Vector<FCursorHistorySampleV2> &;
        auto Clear() -> void;
    };

    DefinePointers(FCursorHistoryListenerV2)

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_CURSOR_HISTORY_LISTENER_V2_H
