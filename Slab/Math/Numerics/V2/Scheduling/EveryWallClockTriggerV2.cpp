#include "EveryWallClockTriggerV2.h"

#include <cmath>

namespace Slab::Math::Numerics::V2 {

    FEveryWallClockTriggerV2::FEveryWallClockTriggerV2(const double intervalSeconds) {
        SetIntervalSeconds(intervalSeconds);
    }

    auto FEveryWallClockTriggerV2::Reset(const FSimulationCursorV2 &startCursor) -> void {
        AnchorSeconds = startCursor.WallClockSeconds;
        bInitialized = true;
    }

    auto FEveryWallClockTriggerV2::GetDomain() const -> ETriggerDomainV2 {
        return ETriggerDomainV2::WallClock;
    }

    auto FEveryWallClockTriggerV2::GetNextDueStepAfter(const FSimulationCursorV2 &currentCursor)
            -> std::optional<UIntBig> {
        (void) currentCursor;
        return std::nullopt;
    }

    auto FEveryWallClockTriggerV2::GetNextDueWallClockAfter(const FSimulationCursorV2 &currentCursor)
            -> std::optional<double> {
        if (!bInitialized) {
            AnchorSeconds = currentCursor.WallClockSeconds;
            bInitialized = true;
        }

        if (currentCursor.WallClockSeconds < AnchorSeconds) return AnchorSeconds;

        const auto delta = currentCursor.WallClockSeconds - AnchorSeconds;
        const auto k = std::floor(delta / IntervalSeconds);
        return AnchorSeconds + (k + 1.0) * IntervalSeconds;
    }

    auto FEveryWallClockTriggerV2::IsDue(const FSimulationCursorV2 &cursor) const -> bool {
        (void) cursor;
        // Wall-clock triggers are evaluated by scheduler crossing checks, not equality predicates.
        return false;
    }

    auto FEveryWallClockTriggerV2::GetIntervalSeconds() const -> double {
        return IntervalSeconds;
    }

    auto FEveryWallClockTriggerV2::SetIntervalSeconds(const double intervalSeconds) -> void {
        IntervalSeconds = intervalSeconds > 0.0 ? intervalSeconds : 0.001;
    }

} // namespace Slab::Math::Numerics::V2
