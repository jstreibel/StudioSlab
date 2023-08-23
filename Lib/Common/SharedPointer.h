//
// Created by joao on 21/08/23.
//

#ifndef STUDIOSLAB_SHAREDPOINTER_H
#define STUDIOSLAB_SHAREDPOINTER_H


template <typename T>
class SharedPointer {
private:
    T* ptr;
    int* count;

public:
    // Constructor
    explicit SharedPointer(T* p = nullptr);

    // Copy constructor
    SharedPointer(const SharedPointer& other);

    // Move constructor
    SharedPointer(SharedPointer&& other) noexcept;

    // Assignment operator
    SharedPointer& operator=(const SharedPointer& other);

    // Move assignment operator
    SharedPointer& operator=(SharedPointer&& other) noexcept;

    // Destructor
    ~SharedPointer();

    T& operator* () const;
    T* operator->() const;


};

template <typename T>
inline bool operator==(const SharedPointer<T>& a, nullptr_t) noexcept
{ return !a; }

template<typename T>
SharedPointer<T>::SharedPointer(T *p) {
    ptr = p;
    if (ptr) {
        count = new int(1);
    } else {
        count = new int(0);
    }
}

template<typename T>
SharedPointer<T>::SharedPointer(const SharedPointer &other) {
    ptr = other.ptr;
    count = other.count;
    (*count)++;
}

template<typename T>
SharedPointer<T>::SharedPointer(SharedPointer &&other) noexcept {
    ptr = other.ptr;
    count = other.count;
    other.ptr = nullptr;
    other.count = nullptr;
}

template<typename T>
SharedPointer<T> &SharedPointer<T>::operator=(const SharedPointer &other) {
    if (this == &other) return *this;

    // Decrease current count
    (*count)--;

    // If no more references, delete resources
    if (*count == 0) {
        delete ptr;
        delete count;
    }

    // Copy other's resources
    ptr = other.ptr;
    count = other.count;
    (*count)++;

    return *this;
}

template<typename T>
SharedPointer<T> &SharedPointer<T>::operator=(SharedPointer &&other) noexcept {
    if (this == &other) return *this;

    // Delete current resources
    delete ptr;
    delete count;

    // Move other's resources
    ptr = other.ptr;
    count = other.count;
    other.ptr = nullptr;
    other.count = nullptr;

    return *this;
}

template<typename T>
SharedPointer<T>::~SharedPointer() {
    (*count)--;
    if (*count == 0) {
        delete ptr;
        delete count;
    }
}

template<typename T>
T *SharedPointer<T>::operator->() const { return ptr; }

template<typename T>
T &SharedPointer<T>::operator*() const { return *ptr; }

#endif //STUDIOSLAB_SHAREDPOINTER_H
