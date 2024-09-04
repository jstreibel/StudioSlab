//
// Created by joao on 7/1/24.
//

#ifndef STUDIOSLAB_CONTAINERS_H
#define STUDIOSLAB_CONTAINERS_H

#include "Pointer.h"
#include <algorithm>

namespace Slab {
    template<typename T>
    concept Iterable = requires(T t) {
        { t.begin() } -> std::input_iterator;
        { t.end() } -> std::input_iterator;
    };

    bool Contains(auto container, const auto &element) {
        return std::find(container.begin(), container.end(), element) != container.end();
    }

    template<Iterable Container, typename T>
    bool ContainsReference(const Container& container, const Volatile<T>& target) {
        if (auto targetSharedPtr = target.lock()) {
            return std::any_of(container.begin(), container.end(), [&targetSharedPtr](const std::weak_ptr<T>& weakPtr) {
                if (auto sharedPtr = weakPtr.lock()) {
                    return sharedPtr == targetSharedPtr;
                }
                return false;
            });
        }
        return false;
    }

    template<typename T>
    bool Find(auto container, bool(*compareFunc)(const T &)) {
        return std::find_if(container.begin(), container.end(), compareFunc) != container.end();
    }

}

#endif //STUDIOSLAB_CONTAINERS_H
