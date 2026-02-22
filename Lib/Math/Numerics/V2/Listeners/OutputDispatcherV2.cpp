#include "OutputDispatcherV2.h"

namespace Slab::Math::Numerics::V2 {

    auto FOutputDispatcherV2::DispatchSample(const FSubscriptionV2 &subscription, const FSimulationEventV2 &event) const -> void {
        if (subscription.Listener == nullptr) return;

        switch (subscription.DeliveryMode) {
        case EDeliveryModeV2::Synchronous:
        case EDeliveryModeV2::LatestOnly:
        case EDeliveryModeV2::Buffered:
            subscription.Listener->OnSample(event);
            return;
        }
    }

    auto FOutputDispatcherV2::DispatchRunStarted(const Vector<FSubscriptionV2> &subscriptions,
                                                 const FSimulationEventV2 &event) const -> void {
        for (const auto &subscription : subscriptions) {
            if (!subscription.bWantsInitialEvent || subscription.Listener == nullptr) continue;
            subscription.Listener->OnRunStarted(event);
        }
    }

    auto FOutputDispatcherV2::DispatchScheduled(const Vector<FSubscriptionV2> &subscriptions,
                                                const Vector<size_t> &dueIndices,
                                                const FSimulationEventV2 &event) const -> void {
        for (const auto index : dueIndices) {
            if (index >= subscriptions.size()) continue;
            DispatchSample(subscriptions[index], event);
        }
    }

    auto FOutputDispatcherV2::DispatchRunFinished(const Vector<FSubscriptionV2> &subscriptions,
                                                  const FSimulationEventV2 &event) const -> bool {
        bool success = true;

        for (const auto &subscription : subscriptions) {
            if (!subscription.bWantsFinalEvent || subscription.Listener == nullptr) continue;
            success = subscription.Listener->OnRunFinished(event) && success;
        }

        return success;
    }

} // namespace Slab::Math::Numerics::V2
