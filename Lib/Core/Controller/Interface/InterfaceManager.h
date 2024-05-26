//
// Created by joao on 10/17/21.
//

#ifndef FIELDS_INTERFACEMANAGER_H
#define FIELDS_INTERFACEMANAGER_H


#include "Interface.h"


namespace Slab::Core {

    class InterfaceManager {
        static InterfaceManager *instance;

        Vector<Pointer<Interface>> interfaces;

    public:
        static auto getInstance() -> InterfaceManager &;

        // static auto NewInterface(String name, InterfaceOwner *owner) -> Interface::Ptr;

        void registerInterface(const Pointer<Interface> &anInterface);

        auto getInterfaces() -> Vector<Pointer<const Interface>>;

        auto getInterface(const char *string) -> Pointer<const Interface>;

        void feedInterfaces(const CLVariablesMap &vm);

        auto renderAsPythonDictionaryEntries() -> Str;

        auto renderParametersToString(const StrVector &params, const Str &separator = " ",
                                      bool longName = true) const -> Str;

        auto getParametersValues(const StrVector &params) const -> Vector<Pair<Str, Str>>;

        auto getParameter(const Str &name) const -> const Parameter &;

    };


}


#endif //FIELDS_INTERFACEMANAGER_H
