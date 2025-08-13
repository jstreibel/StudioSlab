//
// Created by joao on 8/09/23.
//

#include "ThreadPool.h"


namespace Slab::Core {
    ThreadPool::ThreadPool(size_t NumThreads) {
        for (size_t i = 0; i < NumThreads; ++i) {
            Workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> Lock(QueueMutex);
                        Condition.wait(Lock, [this] { return !Tasks.empty() || bTerminate; });
                        if (bTerminate) return;
                        task = std::move(Tasks.front());
                        Tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    ThreadPool::~ThreadPool() {
        {
            std::unique_lock<std::mutex> Lock(QueueMutex);
            bTerminate = true;
        }
        Condition.notify_all();
        for (std::thread& Worker : Workers) {
            Worker.join();
        }
    }

    void ThreadPool::Enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> Lock(QueueMutex);
            Tasks.push(task);
        }
        Condition.notify_one();
    }
} // Core