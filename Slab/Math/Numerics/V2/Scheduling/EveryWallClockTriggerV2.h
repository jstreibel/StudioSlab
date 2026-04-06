#ifndef STUDIOSLAB_EVERY_WALL_CLOCK_TRIGGER_V2_H
#define STUDIOSLAB_EVERY_WALL_CLOCK_TRIGGER_V2_H

#include "TriggerV2.h"

namespace Slab::Math::Numerics::V2 {

    class FEveryWallClockTriggerV2 final : public ITriggerV2 {
        double IntervalSeconds = 0.1;
        double AnchorSeconds = 0.0;
        bool bInitialized = false;

    public:
        explicit FEveryWallClockTriggerV2(double intervalSeconds = 0.1);

        auto Reset(const FSimulationCursorV2 &startCursor) -> void override;

        [[nodiscard]] auto GetDomain() const -> ETriggerDomainV2 override;
        [[nodiscard]] auto GetNextDueStepAfter(const FSimulationCursorV2 &currentCursor)
                -> std::optional<UIntBig> override;
        [[nodiscard]] auto GetNextDueWallClockAfter(const FSimulationCursorV2 &currentCursor)
                -> std::optional<double> override;
        [[nodiscard]] auto IsDue(const FSimulationCursorV2 &cursor) const -> bool override;

        [[nodiscard]] auto GetIntervalSeconds() const -> double;
        auto SetIntervalSeconds(double intervalSeconds) -> void;
    };

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_EVERY_WALL_CLOCK_TRIGGER_V2_H
