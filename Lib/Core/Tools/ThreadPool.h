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
        Vector<std::thread> Workers;
        std::queue<std::function<void()>> Tasks;
        Mutex QueueMutex;
        std::condition_variable Condition;
        bool bTerminate = false;

    public:
        ThreadPool(size_t NumThreads);

        ~ThreadPool();

        void Enqueue(std::function<void()> task);

    };

} // Slab::Core

#endif //STUDIOSLAB_THREADPOOL_H
