//
// Created by joao on 8/09/23.
//

#include "ThreadPool.h"


namespace Slab::Core {
    namespace Tools {
        ThreadPool::ThreadPool(size_t numThreads) {
            for (size_t i = 0; i < numThreads; ++i) {
                workers.emplace_back([this] {
                    while (true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(queueMutex);
                            condition.wait(lock, [this] { return !tasks.empty() || terminate; });
                            if (terminate) return;
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                        task();
                    }
                });
            }
        }

        ThreadPool::~ThreadPool() {
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                terminate = true;
            }
            condition.notify_all();
            for (std::thread& worker : workers) {
                worker.join();
            }
        }

        void ThreadPool::enqueue(std::function<void()> task) {
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                tasks.push(task);
            }
            condition.notify_one();
        }
    } // Core
} // Tools