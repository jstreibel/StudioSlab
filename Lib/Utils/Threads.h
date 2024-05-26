//
// Created by joao on 26/05/24.
//

#ifndef STUDIOSLAB_THREADS_H
#define STUDIOSLAB_THREADS_H

#include <mutex>

namespace Slab {
    using Mutex = std::mutex;

    template<typename T>
    using LockGuard = std::lock_guard<Mutex>;
}

#endif //STUDIOSLAB_THREADS_H
