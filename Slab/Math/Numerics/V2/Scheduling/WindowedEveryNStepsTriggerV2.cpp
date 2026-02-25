#include "WindowedEveryNStepsTriggerV2.h"

namespace Slab::Math::Numerics::V2 {

    FWindowedEveryNStepsTriggerV2::FWindowedEveryNStepsTriggerV2(const UIntBig intervalSteps,
                                                                 const UIntBig windowStartStep,
                                                                 const std::optional<UIntBig> windowEndStep)
    : IntervalSteps(intervalSteps > 0 ? intervalSteps : 1)
    , WindowStartStep(windowStartStep)
    , WindowEndStep(windowEndStep) {
    }

    auto FWindowedEveryNStepsTriggerV2::Reset(const FSimulationCursorV2 &startCursor) -> void {
        (void) startCursor;
        bInitialized = true;
    }

    auto FWindowedEveryNStepsTriggerV2::GetDomain() const -> ETriggerDomainV2 {
        return ETriggerDomainV2::Step;
    }

    auto FWindowedEveryNStepsTriggerV2::GetNextDueStepAfter(const FSimulationCursorV2 &currentCursor)
            -> std::optional<UIntBig> {
        if (!bInitialized) bInitialized = true;

        if (WindowEndStep.has_value() && *WindowEndStep < WindowStartStep) return std::nullopt;

        if (currentCursor.Step < WindowStartStep) {
            if (WindowEndStep.has_value() && WindowStartStep > *WindowEndStep) return std::nullopt;
            return WindowStartStep;
        }

        const auto delta = currentCursor.Step - WindowStartStep;
        const auto k = delta / IntervalSteps;
        const auto next = WindowStartStep + (k + 1) * IntervalSteps;

        if (WindowEndStep.has_value() && next > *WindowEndStep) return std::nullopt;
        return next;
    }

    auto FWindowedEveryNStepsTriggerV2::IsDue(const FSimulationCursorV2 &cursor) const -> bool {
        if (!bInitialized) return false;
        if (cursor.Step < WindowStartStep) return false;
        if (WindowEndStep.has_value() && cursor.Step > *WindowEndStep) return false;

        return ((cursor.Step - WindowStartStep) % IntervalSteps) == 0;
    }

    auto FWindowedEveryNStepsTriggerV2::GetIntervalSteps() const -> UIntBig {
        return IntervalSteps;
    }

    auto FWindowedEveryNStepsTriggerV2::GetWindowStartStep() const -> UIntBig {
        return WindowStartStep;
    }

    auto FWindowedEveryNStepsTriggerV2::GetWindowEndStep() const -> std::optional<UIntBig> {
        return WindowEndStep;
    }

} // namespace Slab::Math::Numerics::V2
