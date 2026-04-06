#include "EveryNStepsTriggerV2.h"

namespace Slab::Math::Numerics::V2 {

    FEveryNStepsTriggerV2::FEveryNStepsTriggerV2(const UIntBig intervalSteps)
    : IntervalSteps(intervalSteps > 0 ? intervalSteps : 1) {
    }

    auto FEveryNStepsTriggerV2::Reset(const FSimulationCursorV2 &startCursor) -> void {
        AnchorStep = startCursor.Step;
        bInitialized = true;
    }

    auto FEveryNStepsTriggerV2::GetDomain() const -> ETriggerDomainV2 {
        return ETriggerDomainV2::Step;
    }

    auto FEveryNStepsTriggerV2::GetNextDueStepAfter(const FSimulationCursorV2 &currentCursor) -> std::optional<UIntBig> {
        if (!bInitialized) {
            AnchorStep = currentCursor.Step;
            bInitialized = true;
        }

        if (currentCursor.Step < AnchorStep) {
            return AnchorStep;
        }

        const auto delta = currentCursor.Step - AnchorStep;
        const auto k = delta / IntervalSteps;
        return AnchorStep + (k + 1) * IntervalSteps;
    }

    auto FEveryNStepsTriggerV2::IsDue(const FSimulationCursorV2 &cursor) const -> bool {
        if (!bInitialized) return false;
        if (cursor.Step <= AnchorStep) return false;

        return ((cursor.Step - AnchorStep) % IntervalSteps) == 0;
    }

    auto FEveryNStepsTriggerV2::GetIntervalSteps() const -> UIntBig {
        return IntervalSteps;
    }

    auto FEveryNStepsTriggerV2::SetIntervalSteps(const UIntBig intervalSteps) -> void {
        IntervalSteps = intervalSteps > 0 ? intervalSteps : 1;
    }

} // namespace Slab::Math::Numerics::V2
