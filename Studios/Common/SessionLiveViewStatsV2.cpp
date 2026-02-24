#include "SessionLiveViewStatsV2.h"

#include <algorithm>

namespace Slab::Studios::Common {

    auto ToDisplayString(const Math::Numerics::V2::EEventReasonV2 reason) -> Str {
        using EReason = Math::Numerics::V2::EEventReasonV2;

        switch (reason) {
        case EReason::Initial: return "Initial";
        case EReason::Scheduled: return "Scheduled";
        case EReason::Forced: return "Forced";
        case EReason::Final: return "Final";
        case EReason::AbortFinal: return "AbortFinal";
        }

        return "Unknown";
    }

    auto AppendSessionLiveViewStats(
        const TPointer<Graphics::FGUIWindow> &guiWindow,
        const Str &title,
        const std::optional<Math::LiveData::V2::FSessionTelemetryV2> &telemetry,
        const bool bHasBoundSession,
        const bool bLeaseAcquired,
        const UIntBig maxSteps,
        const std::optional<Str> &extraLine) -> void {
        if (guiWindow == nullptr) return;

        guiWindow->AddVolatileStat(title);
        guiWindow->AddVolatileStat("");

        if (!telemetry.has_value()) {
            guiWindow->AddVolatileStat("Waiting for telemetry...");
            return;
        }

        const auto &t = *telemetry;

        guiWindow->AddVolatileStat("Step: " + ToStr(t.Cursor.Step));
        if (t.Cursor.SimulationTime.has_value()) {
            guiWindow->AddVolatileStat("t: " + ToStr(*t.Cursor.SimulationTime, 6, true));
        }

        guiWindow->AddVolatileStat("Version: " + ToStr(t.PublishedVersion));
        guiWindow->AddVolatileStat("Reason: " + ToDisplayString(t.LastReason));
        guiWindow->AddVolatileStat(Str("Bound session: ") + (bHasBoundSession ? "yes" : "no"));
        guiWindow->AddVolatileStat(Str("Lease this frame: ") + (bLeaseAcquired ? "yes" : "no"));
        guiWindow->AddVolatileStat(Str("State present: ") + (t.bHasState ? "yes" : "no"));

        if (extraLine.has_value() && !extraLine->empty()) {
            guiWindow->AddVolatileStat(*extraLine);
        }

        if (maxSteps > 0) {
            const auto progress = static_cast<DevFloat>(
                std::min<UIntBig>(t.Cursor.Step, maxSteps)) / static_cast<DevFloat>(maxSteps);
            guiWindow->AddVolatileStat("Progress: " + ToStr(progress * 100.0, 2, true) + "%");
        }
    }

} // namespace Slab::Studios::Common
