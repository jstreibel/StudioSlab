#ifndef STUDIOSLAB_WINDOWED_EVERY_N_STEPS_TRIGGER_V2_H
#define STUDIOSLAB_WINDOWED_EVERY_N_STEPS_TRIGGER_V2_H

#include "TriggerV2.h"

namespace Slab::Math::Numerics::V2 {

    class FWindowedEveryNStepsTriggerV2 final : public ITriggerV2 {
        UIntBig IntervalSteps = 1;
        UIntBig WindowStartStep = 0;
        std::optional<UIntBig> WindowEndStep = std::nullopt;
        bool bInitialized = false;

    public:
        FWindowedEveryNStepsTriggerV2(UIntBig intervalSteps,
                                      UIntBig windowStartStep,
                                      std::optional<UIntBig> windowEndStep = std::nullopt);

        auto Reset(const FSimulationCursorV2 &startCursor) -> void override;

        [[nodiscard]] auto GetDomain() const -> ETriggerDomainV2 override;

        [[nodiscard]] auto GetNextDueStepAfter(const FSimulationCursorV2 &currentCursor)
                -> std::optional<UIntBig> override;

        [[nodiscard]] auto IsDue(const FSimulationCursorV2 &cursor) const -> bool override;

        [[nodiscard]] auto GetIntervalSteps() const -> UIntBig;
        [[nodiscard]] auto GetWindowStartStep() const -> UIntBig;
        [[nodiscard]] auto GetWindowEndStep() const -> std::optional<UIntBig>;
    };

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_WINDOWED_EVERY_N_STEPS_TRIGGER_V2_H
