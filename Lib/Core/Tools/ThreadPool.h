//
// Created by joao on 8/09/23.
//

#ifndef STUDIOSLAB_THREADPOOL_H
#define STUDIOSLAB_THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace Slab::Core {
    namespace Tools {

        class ThreadPool {
            std::vector<std::thread> workers;
            std::queue<std::function<void()>> tasks;
            std::mutex queueMutex;
            std::condition_variable condition;
            bool terminate = false;

        public:
            ThreadPool(size_t numThreads);

            ~ThreadPool();

            void enqueue(std::function<void()> task);

        };

    } // Core
} // Tools

#endif //STUDIOSLAB_THREADPOOL_H
