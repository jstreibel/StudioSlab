//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_SINGLETON_H
#define STUDIOSLAB_SINGLETON_H

#include <utility>

#include "Types.h"
// #include "Core/Tools/Log.h"

namespace Slab {

    template<typename T>
    class Singleton {
    protected:
        const Str name;
        static T *singleInstance;

        explicit Singleton(Str name) : name(std::move(name)) {
            // Log::Info("Singleton \"") << name << "\" has been instantiated." << Log::Flush;
        };

        virtual ~Singleton() = default;

    public:
        Singleton(const Singleton &) = delete;

        static T &GetInstance() {
            if (Singleton::singleInstance == nullptr) singleInstance = new T();;

            return *singleInstance;
        }

        static void Terminate() {
            delete singleInstance;
            singleInstance = nullptr;
        }

        Singleton &operator=(const Singleton &) = delete;
    };

    template<typename T>
    T *Singleton<T>::singleInstance = nullptr;

/*
template <typename T>
class DerivableSingleton : public Singleton<T, true> {
protected:
    explicit DerivableSingleton(const Str &name) : Singleton<T,true>(name) {}
public:

    template< typename ToSuper >
    static ToSuper& Initialize() {
        if(Singleton<T,true>::singleInstance != nullptr)
            throw Str("Singleton '") + Singleton<T,true>::singleInstance->name + "' already initialized.";


        Singleton<T,true>::singleInstance = new ToSuper();

        return reinterpret_cast<ToSuper&>(*Singleton<T, true>::singleInstance);
    }

    template< typename ToSuper >
    static ToSuper& GetInstanceSuper() {
        auto &me = Singleton<T,true>::GetInstance();

        return dynamic_cast<ToSuper&>(me);
    }

};
*/

}

#endif //STUDIOSLAB_SINGLETON_H
