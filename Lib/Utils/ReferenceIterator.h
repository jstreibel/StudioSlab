//
// Created by joao on 7/1/24.
//

#ifndef STUDIOSLAB_REFERENCEITERATOR_H
#define STUDIOSLAB_REFERENCEITERATOR_H

#include <iterator>
#include <Core/Tools/Log.h>

#include "Containers.h"

namespace Slab {

    #define SLAB_FUNC(method, ...) [__VA_ARGS__] (const auto &obj) { \
        return obj->method(__VA_ARGS__); }

    #define SLAB_FUNC_RUNTHROUGH(method, ...) [__VA_ARGS__] (const auto &obj) { \
        obj->method(__VA_ARGS__); return false; }

    enum ReferenceIterationPolicy {
        IterateAll=false,
        StopOnFirstResponder=true
    };

    template<Iterable Container, typename FuncTy>
    bool IterateReferences(Container& container, FuncTy func, ReferenceIterationPolicy policy = IterateAll) {
        auto any_responded = false;

        auto it = container.begin();
        while (it != container.end()) {
            if (auto sharedPtr = it->lock()) {
                const bool responded = func(sharedPtr);
                any_responded |= responded;

                if(responded && policy==StopOnFirstResponder) break;

                ++it;
            }
            else {
                it = container.erase(it); // Remove expired pointer and get the next iterator
            }
        }

        return any_responded;
    }
}

#endif //STUDIOSLAB_REFERENCEITERATOR_H
