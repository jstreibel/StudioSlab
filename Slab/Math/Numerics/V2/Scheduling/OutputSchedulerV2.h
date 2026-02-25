#ifndef STUDIOSLAB_OUTPUT_SCHEDULER_V2_H
#define STUDIOSLAB_OUTPUT_SCHEDULER_V2_H

#include "Math/Numerics/V2/Listeners/ListenerV2.h"
#include "Math/Numerics/V2/Scheduling/TriggerV2.h"

#include <optional>

namespace Slab::Math::Numerics::V2 {

    class FOutputSchedulerV2 final {
    public:
        auto Reset(Vector<FSubscriptionV2> &subscriptions, const FSimulationCursorV2 &startCursor) const -> void;

        [[nodiscard]] auto ComputeNextScheduledStepAfter(const Vector<FSubscriptionV2> &subscriptions,
                                                         const FSimulationCursorV2 &cursor) const
                -> std::optional<UIntBig>;

        [[nodiscard]] auto CollectDueSubscriptions(const Vector<FSubscriptionV2> &subscriptions,
                                                   const FSimulationCursorV2 &cursor) const -> Vector<size_t>;

        [[nodiscard]] auto CollectDueSubscriptionsBetween(const Vector<FSubscriptionV2> &subscriptions,
                                                          const FSimulationCursorV2 &prevCursor,
                                                          const FSimulationCursorV2 &currCursor) const
                -> Vector<size_t>;
    };

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_OUTPUT_SCHEDULER_V2_H
