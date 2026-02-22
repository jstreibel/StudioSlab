#include "OutputSchedulerV2.h"

#include <algorithm>

namespace Slab::Math::Numerics::V2 {

    auto FOutputSchedulerV2::Reset(Vector<FSubscriptionV2> &subscriptions, const FSimulationCursorV2 &startCursor) const -> void {
        for (auto &subscription : subscriptions) {
            if (subscription.Trigger == nullptr) continue;
            subscription.Trigger->Reset(startCursor);
        }
    }

    auto FOutputSchedulerV2::ComputeNextScheduledStepAfter(const Vector<FSubscriptionV2> &subscriptions,
                                                           const FSimulationCursorV2 &cursor) const
            -> std::optional<UIntBig> {
        std::optional<UIntBig> nextDue = std::nullopt;

        for (const auto &subscription : subscriptions) {
            if (subscription.Trigger == nullptr || subscription.Listener == nullptr) continue;
            if (subscription.Trigger->GetDomain() != ETriggerDomainV2::Step) continue;

            const auto candidate = subscription.Trigger->GetNextDueStepAfter(cursor);
            if (!candidate.has_value()) continue;
            if (*candidate <= cursor.Step) continue;

            if (!nextDue.has_value()) nextDue = *candidate;
            else nextDue = std::min(*nextDue, *candidate);
        }

        return nextDue;
    }

    auto FOutputSchedulerV2::CollectDueSubscriptions(const Vector<FSubscriptionV2> &subscriptions,
                                                     const FSimulationCursorV2 &cursor) const -> Vector<size_t> {
        Vector<size_t> due;

        for (size_t i = 0; i < subscriptions.size(); ++i) {
            const auto &subscription = subscriptions[i];
            if (subscription.Trigger == nullptr || subscription.Listener == nullptr) continue;
            if (subscription.Trigger->GetDomain() != ETriggerDomainV2::Step) continue;
            if (subscription.Trigger->IsDue(cursor)) due.push_back(i);
        }

        return due;
    }

} // namespace Slab::Math::Numerics::V2
