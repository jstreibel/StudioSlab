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
    using TPointer = std::shared_ptr<T>;

    template <typename T>
    using TUnique = std::unique_ptr<T>;

    template <typename T>
    using Reference = TPointer<TPointer<T>>;

    template <typename T>
    using TVolatile = std::weak_ptr<T>;

    template<typename Derived>
    inline auto DynamicPointerCast(auto base_pointer) {
        return std::dynamic_pointer_cast<Derived>(base_pointer);
    }

    template<typename Derived>
    inline auto ConstPointerCast(auto base_pointer) {
        return std::const_pointer_cast<Derived>(base_pointer);
    }

    template<typename T, typename... Args>
    TPointer<T> New(Args&&... args);

    template<typename T>
    TPointer<T> PointerWrap(T *instance) {
        return TPointer<T>(instance);
    }

    template<typename T>
    TPointer<T> Naked(T &instance) {
        return TPointer<T>(&instance, [](T*){});
    }

    /**
     * Same as Naked.
     * @tparam T
     * @param instance
     * @return
     */
    template<typename T>
    TPointer<T> Dummy(T &instance) {
        return TPointer<T>(&instance, [](T*){});
    }

    template<typename T>
    TPointer<T> DummyPointer_const(T &instance) {
        return TPointer<const T>(&instance, [](T*){});
    }

    template<typename T>
    TPointer<T> NullPtr() {
        T *ptr = nullptr;
        return TPointer<T>(ptr, [](T*){});
    }

    template<typename T, typename... Args>
    TPointer<T> New(Args&&... args) {
        // return Pointer<T>(new T(std::forward<Args>(args)...));
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

#define DefinePointers(Type) \
    typedef ::Slab::TPointer<Type> Type##_ptr; \
    typedef ::Slab::TPointer<const Type> Type##_constptr; \
    typedef ::Slab::TVolatile<Type> Type##_ref;          \
    typedef ::Slab::TVolatile<const Type> Type##_constref;
}

#endif //STUDIOSLAB_POINTER_H
