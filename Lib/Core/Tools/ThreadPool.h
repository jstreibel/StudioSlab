//
// Created by joao on 8/09/23.
//

#ifndef STUDIOSLAB_THREADPOOL_H
#define STUDIOSLAB_THREADPOOL_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "Utils/Arrays.h"
#include "Utils/Threads.h"

namespace Slab::Core {

    class ThreadPool {
        Vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        Mutex queueMutex;
        std::condition_variable condition;
        bool terminate = false;

    public:
        ThreadPool(size_t numThreads);

        ~ThreadPool();

        void enqueue(std::function<void()> task);

    };

} // Slab::Core

#endif //STUDIOSLAB_THREADPOOL_H
