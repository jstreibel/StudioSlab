#ifndef STUDIOSLAB_EVERY_N_STEPS_TRIGGER_V2_H
#define STUDIOSLAB_EVERY_N_STEPS_TRIGGER_V2_H

#include "TriggerV2.h"

namespace Slab::Math::Numerics::V2 {

    class FEveryNStepsTriggerV2 final : public ITriggerV2 {
        UIntBig IntervalSteps = 1;
        UIntBig AnchorStep = 0;
        bool bInitialized = false;

    public:
        explicit FEveryNStepsTriggerV2(UIntBig intervalSteps = 1);

        auto Reset(const FSimulationCursorV2 &startCursor) -> void override;

        [[nodiscard]] auto GetDomain() const -> ETriggerDomainV2 override;

        [[nodiscard]] auto GetNextDueStepAfter(const FSimulationCursorV2 &currentCursor)
                -> std::optional<UIntBig> override;

        [[nodiscard]] auto IsDue(const FSimulationCursorV2 &cursor) const -> bool override;

        [[nodiscard]] auto GetIntervalSteps() const -> UIntBig;
        auto SetIntervalSteps(UIntBig intervalSteps) -> void;
    };

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_EVERY_N_STEPS_TRIGGER_V2_H
