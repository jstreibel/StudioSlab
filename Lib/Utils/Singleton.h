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
    class FSingleton {
    protected:
        const Str name;
        static T *singleInstance;

        explicit FSingleton(Str name) : name(std::move(name)) {
            // Log::Info("Singleton \"") << name << "\" has been instantiated." << Log::Flush;
        };

        virtual ~FSingleton() = default;

    public:
        FSingleton(const FSingleton &) = delete;

        static T &GetInstance() {
            if (FSingleton::singleInstance == nullptr) singleInstance = new T();;

            return *singleInstance;
        }

        static void Terminate() {
            delete singleInstance;
            singleInstance = nullptr;
        }

        FSingleton &operator=(const FSingleton &) = delete;
    };

    template<typename T>
    T *FSingleton<T>::singleInstance = nullptr;

    template<typename T>
    using Singleton [[deprecated("Use FSingleton")]] = FSingleton<T>;

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
