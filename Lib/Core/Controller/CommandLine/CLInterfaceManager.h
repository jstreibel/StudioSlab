//
// Created by joao on 10/17/21.
//

#ifndef FIELDS_INTERFACEMANAGER_H
#define FIELDS_INTERFACEMANAGER_H


#include "Core/Controller/CommandLine/CLInterface.h"


namespace Slab::Core {

    class CLInterfaceManager {
        static CLInterfaceManager *instance;

        Vector<Pointer<CLInterface>> interfaces;

    public:
        static auto getInstance() -> CLInterfaceManager &;

        // static auto NewInterface(String name, InterfaceOwner *owner) -> Interface::Ptr;

        void registerInterface(const Pointer<CLInterface> &anInterface);

        auto getInterfaces() -> Vector<Pointer<const CLInterface>>;

        auto getInterface(const char *string) -> Pointer<const CLInterface>;

        void feedInterfaces(const CLVariablesMap &vm);

        auto renderAsPythonDictionaryEntries() -> Str;

        auto renderParametersToString(const StrVector &params, const Str &separator = " ",
                                      bool longName = true) const -> Str;

        auto getParametersValues(const StrVector &params) const -> Vector<Pair<Str, Str>>;

        auto getParameter(const Str &name) const -> Pointer<const CLParameter>;

    };


}


#endif //FIELDS_INTERFACEMANAGER_H
