//
// Created by joao on 26/05/24.
//

#ifndef STUDIOSLAB_THREADS_H
#define STUDIOSLAB_THREADS_H

#include <condition_variable>
#include <mutex>
#include <thread>

namespace Slab {
    using Mutex = std::mutex;
    using Thread = std::thread;
    using Condition = std::condition_variable;

    template<typename T>
    using Atomic = std::atomic<T>;

    template<typename T>
    using LockGuard = std::lock_guard<Mutex>;
}

#endif //STUDIOSLAB_THREADS_H
