//
// Created by joao on 20/05/24.
//

#ifndef STUDIOSLAB_POINTER_H
#define STUDIOSLAB_POINTER_H

#include "Numbers.h"

namespace Slab {

    template<typename T> class Pointer;

    template<typename T, typename... Args>
    Pointer<T> New(Args&&... args);


    template<typename T>
    class Pointer {
    private:
        T* ptr;
        Count* ref_count;

        void release() {
            if (ref_count) {
                --(*ref_count);
                if (*ref_count == 0) {
                    delete ptr;
                    delete ref_count;
                }
            }
        }

    public:
        // Default constructor
        Pointer() : ptr(nullptr), ref_count(new Count(1)) {}

        // Parameterized constructor
        explicit Pointer(T* p) : ptr(p), ref_count(new Count(1)) {}

        // Copy constructor
        Pointer(const Pointer& other) : ptr(other.ptr), ref_count(other.ref_count) {
            ++(*ref_count);
        }

        // Move constructor
        Pointer(Pointer&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count) {
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
                ++(*ref_count);
            }
            return *this;
        }

        // Move assignment operator
        Pointer& operator=(Pointer&& other) noexcept {
            if (this != &other) {
                release();
                ptr = other.ptr;
                ref_count = other.ref_count;
                other.ptr = nullptr;
                other.ref_count = nullptr;
            }
            return *this;
        }

        // Equality operators
        bool operator==(const Pointer& other) const {
            return ptr == other.ptr;
        }
        bool operator==(const T* other) const {
            return ptr == other;
        }

        // Inequality operator
        bool operator!=(const Pointer& other) const {
            return ptr != other.ptr;
        }
        bool operator!=(const T* other) const {
            return ptr != other;
        }

        // Dereference operator
        T& operator*() const {
            return *ptr;
        }

        // Arrow operator
        T* operator->() const {
            return ptr;
        }

        // Get the use count
        Count use_count() const {
            return ref_count ? *ref_count : 0;
        }

        // Reset the pointer
        void reset(T* p = nullptr) {
            release();
            ptr = p;
            ref_count = new Count(1);
        }
    };

    template<typename T, typename... Args>
    Pointer<T> New(Args&&... args) {
        return Pointer<T>(new T(std::forward<Args>(args)...));
    }
}

#endif //STUDIOSLAB_POINTER_H
