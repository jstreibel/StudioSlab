#include "OutputDispatcherV2.h"

namespace Slab::Math::Numerics::V2 {

    FOutputDispatcherV2::~FOutputDispatcherV2() {
        StopLatestOnlyWorkers();
    }

    auto FOutputDispatcherV2::GetOrCreateLatestOnlyWorker(const TPointer<IListenerV2> &listener) const
            -> TPointer<FLatestOnlyWorkerV2> {
        if (listener == nullptr) return nullptr;

        std::lock_guard lock(LatestOnlyWorkersMutex);

        const auto key = listener.get();
        const auto it = LatestOnlyWorkers.find(key);
        if (it != LatestOnlyWorkers.end()) return it->second;

        auto worker = New<FLatestOnlyWorkerV2>();
        worker->Listener = listener;
        worker->WorkerThread = std::thread([worker] {
            if (worker == nullptr || worker->Listener == nullptr) return;

            while (true) {
                FSimulationEventV2 event;

                {
                    std::unique_lock lock(worker->Mutex);
                    worker->CV.wait(lock, [&] { return worker->bStop || worker->bHasPending; });

                    if (worker->bStop && !worker->bHasPending) break;

                    event = worker->PendingEvent;
                    worker->bHasPending = false;
                    worker->bBusy = true;
                }

                worker->Listener->OnSample(event);

                {
                    std::lock_guard lock(worker->Mutex);
                    worker->bBusy = false;
                }
                worker->CV.notify_all();
            }
        });
        LatestOnlyWorkers[key] = worker;
        return worker;
    }

    auto FOutputDispatcherV2::DispatchSampleSync(const FSubscriptionV2 &subscription,
                                                 const FSimulationEventV2 &event) const -> void {
        if (subscription.Listener == nullptr) return;
        subscription.Listener->OnSample(event);
    }

    auto FOutputDispatcherV2::DispatchSampleLatestOnly(const FSubscriptionV2 &subscription,
                                                       const FSimulationEventV2 &event) const -> void {
        if (subscription.Listener == nullptr) return;

        const auto worker = GetOrCreateLatestOnlyWorker(subscription.Listener);
        if (worker == nullptr) return;

        {
            std::lock_guard lock(worker->Mutex);
            worker->PendingEvent = event;
            worker->bHasPending = true;
        }
        worker->CV.notify_one();
    }

    auto FOutputDispatcherV2::FlushLatestOnlyWorkers() const -> void {
        Vector<TPointer<FLatestOnlyWorkerV2>> workers;
        {
            std::lock_guard lock(LatestOnlyWorkersMutex);
            workers.reserve(LatestOnlyWorkers.size());
            for (const auto &[_, worker] : LatestOnlyWorkers) workers.push_back(worker);
        }

        for (const auto &worker : workers) {
            if (worker == nullptr) continue;
            std::unique_lock lock(worker->Mutex);
            worker->CV.wait(lock, [&] { return !worker->bHasPending && !worker->bBusy; });
        }
    }

    auto FOutputDispatcherV2::StopLatestOnlyWorkers() -> void {
        Vector<TPointer<FLatestOnlyWorkerV2>> workers;
        {
            std::lock_guard lock(LatestOnlyWorkersMutex);
            for (const auto &[_, worker] : LatestOnlyWorkers) workers.push_back(worker);
            LatestOnlyWorkers.clear();
        }

        for (const auto &worker : workers) {
            if (worker == nullptr) continue;

            {
                std::lock_guard lock(worker->Mutex);
                worker->bStop = true;
            }
            worker->CV.notify_all();

            if (worker->WorkerThread.joinable()) worker->WorkerThread.join();
        }
    }

    auto FOutputDispatcherV2::DispatchSample(const FSubscriptionV2 &subscription, const FSimulationEventV2 &event) const -> void {
        if (subscription.Listener == nullptr) return;

        switch (subscription.DeliveryMode) {
        case EDeliveryModeV2::Synchronous:
            DispatchSampleSync(subscription, event);
            return;
        case EDeliveryModeV2::LatestOnly:
            DispatchSampleLatestOnly(subscription, event);
            return;
        case EDeliveryModeV2::Buffered:
            // Buffered mode is not implemented yet; keep semantics deterministic for now.
            DispatchSampleSync(subscription, event);
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

        FlushLatestOnlyWorkers();

        for (const auto &subscription : subscriptions) {
            if (!subscription.bWantsFinalEvent || subscription.Listener == nullptr) continue;
            success = subscription.Listener->OnRunFinished(event) && success;
        }

        return success;
    }

} // namespace Slab::Math::Numerics::V2
