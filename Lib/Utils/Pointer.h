//
// Created by joao on 20/05/24.
//

#ifndef STUDIOSLAB_POINTER_H
#define STUDIOSLAB_POINTER_H

#include <functional>
#include <mutex>
#include "Numbers.h"

namespace Slab {

    template <typename T>
    using Pointer = std::shared_ptr<T>;

    template<typename Derived>
    inline auto DynamicPointerCast(auto base_pointer) {
        return std::dynamic_pointer_cast<Derived>(base_pointer);;
    }


    template<typename T, typename... Args>
    Pointer<T> New(Args&&... args);

    template<typename T>
    Pointer<T> PointerWrap(T *instance) {
        return Pointer<T>(instance);
    }

    template<typename T>
    Pointer<T> Naked(T &instance) {
        return Pointer<T>(&instance, [](T*){});
    }

    template<typename T>
    Pointer<T> DummyPointer_const(T &instance) {
        return Pointer<const T>(&instance, [](T*){});
    }

    template<typename T>
    Pointer<T> NullPtr() {
        T *ptr = nullptr;
        return Pointer<T>(ptr, [](T*){});
    }

    template<typename T, typename... Args>
    Pointer<T> New(Args&&... args) {
        return Pointer<T>(new T(std::forward<Args>(args)...));
    }

#define DefinePointer(Type) \
    typedef ::Slab::Pointer<Type> Type##_ptr; \
    typedef ::Slab::Pointer<const Type> Type##_constptr;
}

#endif //STUDIOSLAB_POINTER_H
