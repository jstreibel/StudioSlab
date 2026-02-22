#ifndef STUDIOSLAB_OUTPUT_DISPATCHER_V2_H
#define STUDIOSLAB_OUTPUT_DISPATCHER_V2_H

#include "ListenerV2.h"

namespace Slab::Math::Numerics::V2 {

    class FOutputDispatcherV2 final {
        auto DispatchSample(const FSubscriptionV2 &subscription, const FSimulationEventV2 &event) const -> void;

    public:
        auto DispatchRunStarted(const Vector<FSubscriptionV2> &subscriptions,
                                const FSimulationEventV2 &event) const -> void;

        auto DispatchScheduled(const Vector<FSubscriptionV2> &subscriptions,
                               const Vector<size_t> &dueIndices,
                               const FSimulationEventV2 &event) const -> void;

        auto DispatchRunFinished(const Vector<FSubscriptionV2> &subscriptions,
                                 const FSimulationEventV2 &event) const -> bool;
    };

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_OUTPUT_DISPATCHER_V2_H
