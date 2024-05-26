//
// Created by joao on 20/05/24.
//

#ifndef STUDIOSLAB_POINTER_H
#define STUDIOSLAB_POINTER_H

#include <functional>
#include <mutex>
#include "Numbers.h"

// #define USE_THREADSAFE_POINTER

namespace Slab {

    template <typename T>
    using Pointer = std::shared_ptr<T>;

    template<typename T, typename... Args>
    Pointer<T> New(Args&&... args);

    template<typename T>
    Pointer<T> PointerWrap(T *instance) {
        return Pointer<T>(instance);
    }

    template<typename T>
    Pointer<T> DummyPointer(T &instance) {
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

#ifdef USE_THREADSAFE_POINTER
    template <typename T>
    class Pointer {
    public:
        using DeleterType = std::function<void(T*)>;

        // Default constructor
        Pointer() : ptr(nullptr), ref_count(nullptr), deleter(nullptr), mtx(new std::mutex()) {}

        // Constructor with raw pointer
        explicit Pointer(T* p) : ptr(p), ref_count(new size_t(1)), deleter(defaultDeleter), mtx(new std::mutex()) {}

        // Constructor with custom deleter
        Pointer(T* p, DeleterType d) : ptr(p), ref_count(new size_t(1)), deleter(d), mtx(new std::mutex()) {}

        // Copy constructor
        Pointer(const Pointer& other) : ptr(other.ptr), ref_count(other.ref_count), deleter(other.deleter), mtx(other.mtx) {
            std::lock_guard<std::mutex> lock(*mtx);
            if (ref_count) {
                ++(*ref_count);
            }
        }

        // Move constructor
        Pointer(Pointer&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count), deleter(std::move(other.deleter)), mtx(std::move(other.mtx)) {
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }

        // Destructor
        ~Pointer() {
            release();
        }

        // Copy assignment operator
        Pointer& operator=(const Pointer& other) {
            if (this != &other) {
                release();
                std::scoped_lock lock(*this->mtx, *other.mtx);

                ptr = other.ptr;
                ref_count = other.ref_count;
                deleter = other.deleter;
                mtx = other.mtx;
                if (ref_count) {
                    ++(*ref_count);
                }
            }
            return *this;
        }

        // Move assignment operator
        Pointer& operator=(Pointer&& other) noexcept {
            if (this != &other) {
                release();
                std::scoped_lock lock(*this->mtx, *other.mtx);

                ptr = other.ptr;
                ref_count = other.ref_count;
                deleter = std::move(other.deleter);
                mtx = std::move(other.mtx);
                other.ptr = nullptr;
                other.ref_count = nullptr;
            }
            return *this;
        }

        // Conversion constructor for derived to base assignment (non-const)
        template <typename U, typename std::enable_if<std::is_convertible<U*, T*>::value && !std::is_const<T>::value, int>::type = 0>
        Pointer(const Pointer<U>& other) : ptr(other.ptr), ref_count(other.ref_count), deleter(nullptr), mtx(other.mtx) {
            std::lock_guard<std::mutex> lock(*this->mtx);
            if (ref_count) {
                ++(*ref_count);
            }
            deleter = [other_deleter = other.deleter](T* p) {
                other_deleter(static_cast<U*>(p));
            };
        }

        // Conversion constructor for non-const to const assignment
        template <typename U, typename std::enable_if<std::is_convertible<U*, T*>::value && std::is_const<T>::value, int>::type = 0>
        Pointer(const Pointer<U>& other) : ptr(other.ptr), ref_count(other.ref_count), deleter(nullptr), mtx(other.mtx) {
            std::lock_guard<std::mutex> lock(*this->mtx);
            if (ref_count) {
                ++(*ref_count);
            }
            deleter = [other_deleter = other.deleter](const T* p) {
                other_deleter(static_cast<U*>(const_cast<std::remove_const_t<T>*>(p)));
            };
        }

        // Equality operators
        bool operator==(const Pointer& other) const {
            return ptr == other.ptr;
        }
        bool operator==(const T* other) const {
            return ptr == other;
        }
        bool operator==(const T& other) const {
            return ptr == &other;
        }

        // Inequality operators
        bool operator!=(const Pointer& other) const {
            return ptr != other.ptr;
        }
        bool operator!=(const T *other) const {
            return ptr != other;
        }
        bool operator!=(const T &other) const {
            return ptr != &other;
        }

        // Dereference operator
        T& operator*() const {
            return *ptr;
        }

        // Member access operator
        T* operator->() const {
            return ptr;
        }

        // Get the raw pointer
        T* get() const {
            return ptr;
        }

        // Get the use count
        size_t use_count() const {
            std::lock_guard<std::mutex> lock(*mtx);
            return ref_count ? *ref_count : 0;
        }

    private:
        T* ptr;
        size_t* ref_count;
        DeleterType deleter;
        std::shared_ptr<std::mutex> mtx;

        static void defaultDeleter(T* p) {
            delete p;
        }

        void release() {
            if(ref_count == nullptr) {
                assert(ptr == nullptr);
                return;
            }

            std::lock_guard<std::mutex> lock(*mtx);
            if (ref_count && --(*ref_count) == 0) {
                deleter(ptr);
                delete ref_count;

                ref_count = nullptr;
                ptr = nullptr;
            }
        }

        // Friend declaration to allow access to private members
        template <typename U>
        friend class Pointer;
    };

#elif false

    template <typename T>
    class Pointer {
        T* ptr;
        Count * ref_count;
        std::function<void(T*)> deleter;

        static void defaultDeleter(T* p) {
            delete p;
        }

        void release() {
            if (ref_count && --(*ref_count) == 0) {
                deleter(ptr);
                delete ref_count;
            }
        }

        // Friend declaration to allow access to private members
        template <typename U>
        friend class Pointer;

    public:
        using DeleterType = std::function<void(T*)>;

        // Default constructor
        Pointer() : ptr(nullptr), ref_count(nullptr), deleter(nullptr) {}

        // Constructor with raw pointer
        explicit Pointer(T* p) : ptr(p), ref_count(new Count(1)), deleter(defaultDeleter) {}

        // Constructor with custom deleter
        Pointer(T* p, DeleterType d) : ptr(p), ref_count(new Count(1)), deleter(d) {}

        // Copy constructor
        Pointer(const Pointer& other) : ptr(other.ptr), ref_count(other.ref_count), deleter(other.deleter) {
            if (ref_count) {
                ++(*ref_count);
            }
        }

        // Move constructor
        Pointer(Pointer&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count), deleter(std::move(other.deleter)) {
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }

        // Destructor
        ~Pointer() {
            release();
        }

        // Copy assignment operator
        Pointer& operator=(const Pointer& other) {
            if (this != &other) {
                release();
                ptr = other.ptr;
                ref_count = other.ref_count;
                deleter = other.deleter;

                if (ref_count) {
                    ++(*ref_count);
                }
            }
            return *this;
        }

        Pointer& operator=(T *other) {
            if (this->ptr != other) {
                release();
                ptr = other;
                ref_count = new Count(1);
                deleter = defaultDeleter;
            }
            return *this;
        }

        // Move assignment operator
        Pointer& operator=(Pointer&& other) noexcept {
            if (this != &other) {
                release();
                ptr = other.ptr;
                ref_count = other.ref_count;
                deleter = std::move(other.deleter);
                other.ptr = nullptr;
                other.ref_count = nullptr;
            }
            return *this;
        }

        // Conversion constructor for derived to base assignment (non-const)
        template <typename U, typename std::enable_if<std::is_convertible<U*, T*>::value && !std::is_const<T>::value, int>::type = 0>
        Pointer(const Pointer<U>& other) : ptr(other.ptr), ref_count(other.ref_count), deleter(nullptr) {
            if (ref_count) {
                ++(*ref_count);
            }
            deleter = [other_deleter = other.deleter](T* ptr) {
                other_deleter(static_cast<U*>(ptr));
            };
        }

        // Conversion constructor for non-const to const assignment
        template <typename U, typename std::enable_if<std::is_convertible<U*, T*>::value && std::is_const<T>::value, int>::type = 0>
        Pointer(const Pointer<U>& other) : ptr(other.ptr), ref_count(other.ref_count), deleter(nullptr) {
            if (ref_count) {
                ++(*ref_count);
            }
            deleter = [other_deleter = other.deleter](const T* ptr) {
                other_deleter(static_cast<U*>(const_cast<std::remove_const_t<T>*>(ptr)));
            };
        }

        // Equality operators
        bool operator==(const Pointer& other) const {
            return ptr == other.ptr;
        }
        bool operator==(const T* other) const {
            return ptr == other;
        }
        bool operator==(const T& other) const {
            return ptr == &other;
        }

        // Inequality operators
        bool operator!=(const Pointer& other) const {
            return ptr != other.ptr;
        }
        bool operator!=(const T *other) const {
            return ptr != other;
        }
        bool operator!=(const T &other) const {
            return ptr != &other;
        }

        // Dereference operator
        T& operator*() const {
            return *ptr;
        }

        // Member access operator
        T* operator->() const {
            return ptr;
        }

        // Get the raw pointer
        T* get() const {
            return ptr;
        }

        // Get the use count
        Count use_count() const {
            return ref_count ? *ref_count : 0;
        }

    };

#endif

    template<typename T, typename... Args>
    Pointer<T> New(Args&&... args) {
        return Pointer<T>(new T(std::forward<Args>(args)...));
    }

#define DefinePointer(Type) \
    typedef ::Slab::Pointer<Type> Type##_ptr; \
    typedef ::Slab::Pointer<const Type> Type##_constptr;
}

#endif //STUDIOSLAB_POINTER_H
