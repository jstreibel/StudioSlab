//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_SINGLETON_H
#define STUDIOSLAB_SINGLETON_H

#include "Types.h"
#include "IsAbstract.h"
// #include "Base/Tools/Log.h"

template <typename T, bool Abstract=false>
class Singleton {
protected:
    const Str name;
    static T* singleInstance;

    Singleton(Str name) : name(name) {
         // Log::Info("Singleton \"") << name << "\" has been instantiated." << Log::Flush;
    };
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    virtual ~Singleton() {}

public:
    static T& GetInstance() {
        #if Abstract
        if(Singleton::singleInstance == nullptr) singleInstance = new T();
        #endif

        return *singleInstance;
    }

    static void Destroy() {
        if (singleInstance == nullptr) delete singleInstance;
        singleInstance = nullptr;
    }


};

template <typename T, bool Abstract>
T* Singleton<T, Abstract>::singleInstance = nullptr;

template <typename T>
class DerivableSingleton : public Singleton<T, true> {
protected:
    DerivableSingleton(Str name) : Singleton<T,true>(name) {}
public:

    template< typename ToSuper >
    static void Initialize() {
        if(Singleton<T,true>::singleInstance != nullptr)
            throw Str("Singleton '") + Singleton<T,true>::singleInstance->name + "' already initialized.";


        Singleton<T,true>::singleInstance = new ToSuper();
    }

    template< typename ToSuper >
    static ToSuper& GetInstanceSuper() {
        auto &me = Singleton<T,true>::GetInstance();

        return dynamic_cast<ToSuper&>(me);
    }

};


#endif //STUDIOSLAB_SINGLETON_H
