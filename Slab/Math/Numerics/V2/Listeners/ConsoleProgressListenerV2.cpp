#include "ConsoleProgressListenerV2.h"

#include "Core/Tools/Log.h"

#include <algorithm>
#include <utility>

namespace Slab::Math::Numerics::V2 {

    FConsoleProgressListenerV2::FConsoleProgressListenerV2(const std::optional<UIntBig> totalSteps, Str name)
    : Name(std::move(name))
    , TotalSteps(totalSteps) {
    }

    auto FConsoleProgressListenerV2::PrintEvent(const FSimulationEventV2 &event, const Str &prefix) const -> void {
        auto &log = Core::FLog::Info();
        log << Name << " | " << prefix << " | step=" << event.Cursor.Step;

        if (event.Cursor.SimulationTime.has_value()) {
            log << " | t=" << *event.Cursor.SimulationTime;
        }

        if (TotalSteps.has_value()) {
            if (*TotalSteps == 0) {
                log << " | progress=100%";
            } else {
                const auto progress = 100.0 * static_cast<double>(std::min<UIntBig>(event.Cursor.Step, *TotalSteps)) /
                                      static_cast<double>(*TotalSteps);
                log << " | progress=" << progress << "%";
            }
        }

        log << Core::FLog::Flush;
    }

    auto FConsoleProgressListenerV2::OnRunStarted(const FSimulationEventV2 &initialEvent) -> void {
        PrintEvent(initialEvent, "start");
    }

    auto FConsoleProgressListenerV2::OnSample(const FSimulationEventV2 &event) -> void {
        PrintEvent(event, "sample");
    }

    auto FConsoleProgressListenerV2::OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool {
        Str prefix = "finish";
        if (finalEvent.Reason == EEventReasonV2::AbortFinal) prefix = "abort";
        PrintEvent(finalEvent, prefix);
        return true;
    }

    auto FConsoleProgressListenerV2::GetName() const -> Str {
        return Name;
    }

} // namespace Slab::Math::Numerics::V2
