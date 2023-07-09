//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_SINGLETON_H
#define STUDIOSLAB_SINGLETON_H

#include "Types.h"
#include "IsAbstract.h"

template <typename T>
class Singleton {
protected:
    static T* singleInstance;

    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    virtual ~Singleton() {}

public:
    static T& GetInstance() {
        constexpr const auto notAbstract = !IsAbstract<T>::value;
        #if notAbstract
        if(Singleton::singleInstance == nullptr) singleInstance = new T();
        #endif

        return *singleInstance;
    }

    static void Destroy() {
        if (singleInstance == nullptr) delete singleInstance;
        singleInstance = nullptr;
    }
};

template <typename T>
T* Singleton<T>::singleInstance = nullptr;

template <typename T>
class DerivableSingleton : public Singleton<T> {
    const Str name;

protected:
    DerivableSingleton(Str name) : Singleton<T>(), name(name) {}
public:

    template< typename ToSuper >
    static void Initialize() {
        if(Singleton<T>::singleInstance != nullptr) throw "Singleton already initialized.";

        Singleton<T>::singleInstance = new ToSuper();
    }

    template< typename ToSuper >
    static ToSuper& GetInstance() {
        auto &me = Singleton<T>::GetInstance();

        return dynamic_cast<ToSuper&>(me);
    }

};


#endif //STUDIOSLAB_SINGLETON_H
