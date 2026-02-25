#include "OutputSchedulerV2.h"

#include <algorithm>
#include <cmath>

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
        return CollectDueSubscriptionsBetween(subscriptions, cursor, cursor);
    }

    auto FOutputSchedulerV2::CollectDueSubscriptionsBetween(const Vector<FSubscriptionV2> &subscriptions,
                                                            const FSimulationCursorV2 &prevCursor,
                                                            const FSimulationCursorV2 &currCursor) const
            -> Vector<size_t> {
        Vector<size_t> due;

        for (size_t i = 0; i < subscriptions.size(); ++i) {
            const auto &subscription = subscriptions[i];
            if (subscription.Trigger == nullptr || subscription.Listener == nullptr) continue;

            switch (subscription.Trigger->GetDomain()) {
            case ETriggerDomainV2::Step:
                if (subscription.Trigger->IsDue(currCursor)) due.push_back(i);
                break;

            case ETriggerDomainV2::WallClock: {
                const auto nextDue = subscription.Trigger->GetNextDueWallClockAfter(prevCursor);
                if (!nextDue.has_value()) break;

                // Best-effort wall-clock trigger: fire if the next threshold was crossed in this loop.
                if (*nextDue > prevCursor.WallClockSeconds &&
                    *nextDue <= currCursor.WallClockSeconds + 1e-9) {
                    due.push_back(i);
                }
                break;
            }

            case ETriggerDomainV2::SimulationTime:
                // Simulation-time trigger support is not implemented yet in scheduler crossing logic.
                break;
            }
        }

        return due;
    }

} // namespace Slab::Math::Numerics::V2
