#include "CursorHistoryListenerV2.h"

namespace Slab::Math::Numerics::V2 {

    FCursorHistoryListenerV2::FCursorHistoryListenerV2(Str name)
    : Name(std::move(name)) {
    }

    auto FCursorHistoryListenerV2::Store(const FSimulationEventV2 &event) -> void {
        Samples.push_back({event.Cursor, event.Reason});
    }

    auto FCursorHistoryListenerV2::OnRunStarted(const FSimulationEventV2 &initialEvent) -> void {
        Store(initialEvent);
    }

    auto FCursorHistoryListenerV2::OnSample(const FSimulationEventV2 &event) -> void {
        Store(event);
    }

    auto FCursorHistoryListenerV2::OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool {
        Store(finalEvent);
        return true;
    }

    auto FCursorHistoryListenerV2::GetName() const -> Str {
        return Name;
    }

    auto FCursorHistoryListenerV2::GetSamples() const -> const Vector<FCursorHistorySampleV2> & {
        return Samples;
    }

    auto FCursorHistoryListenerV2::Clear() -> void {
        Samples.clear();
    }

} // namespace Slab::Math::Numerics::V2
