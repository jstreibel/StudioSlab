#ifndef STUDIOSLAB_OUTPUT_DISPATCHER_V2_H
#define STUDIOSLAB_OUTPUT_DISPATCHER_V2_H

#include "ListenerV2.h"

#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>

namespace Slab::Math::Numerics::V2 {

    class FOutputDispatcherV2 final {
        struct FLatestOnlyWorkerV2 {
            std::mutex Mutex;
            std::condition_variable CV;
            bool bStop = false;
            bool bHasPending = false;
            bool bBusy = false;
            FSimulationEventV2 PendingEvent;
            TPointer<IListenerV2> Listener = nullptr;
            std::thread WorkerThread;
        };

        mutable std::mutex LatestOnlyWorkersMutex;
        mutable std::map<const IListenerV2 *, TPointer<FLatestOnlyWorkerV2>> LatestOnlyWorkers;

        auto DispatchSampleSync(const FSubscriptionV2 &subscription, const FSimulationEventV2 &event) const -> void;
        auto DispatchSampleLatestOnly(const FSubscriptionV2 &subscription, const FSimulationEventV2 &event) const -> void;
        auto GetOrCreateLatestOnlyWorker(const TPointer<IListenerV2> &listener) const -> TPointer<FLatestOnlyWorkerV2>;
        auto FlushLatestOnlyWorkers() const -> void;
        auto StopLatestOnlyWorkers() -> void;
        auto DispatchSample(const FSubscriptionV2 &subscription, const FSimulationEventV2 &event) const -> void;

    public:
        FOutputDispatcherV2() = default;
        ~FOutputDispatcherV2();
        FOutputDispatcherV2(const FOutputDispatcherV2 &) = delete;
        auto operator=(const FOutputDispatcherV2 &) -> FOutputDispatcherV2 & = delete;
        FOutputDispatcherV2(FOutputDispatcherV2 &&) = delete;
        auto operator=(FOutputDispatcherV2 &&) -> FOutputDispatcherV2 & = delete;

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
