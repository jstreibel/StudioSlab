//
// Created by joao on 10/17/21.
//

#ifndef FIELDS_INTERFACEMANAGER_H
#define FIELDS_INTERFACEMANAGER_H


#include "Core/Controller/CommandLine/CommandLineInterface.h"


namespace Slab::Core {

    class FCommandLineInterfaceManager {
        static FCommandLineInterfaceManager *instance;

        Vector<Pointer<FCommandLineInterface>> interfaces;

    public:
        static auto getInstance() -> FCommandLineInterfaceManager &;

        // static auto NewInterface(String name, InterfaceOwner *owner) -> Interface::Ptr;

        void registerInterface(const Pointer<FCommandLineInterface> &anInterface);

        auto getInterfaces() -> Vector<Pointer<const FCommandLineInterface>>;

        auto getInterface(const char *string) -> Pointer<const FCommandLineInterface>;

        void feedInterfaces(const CLVariablesMap &vm);

        auto renderAsPythonDictionaryEntries() -> Str;

        auto renderParametersToString(const StrVector &params, const Str &separator = " ",
                                      bool longName = true) const -> Str;

        auto getParametersValues(const StrVector &params) const -> Vector<Pair<Str, Str>>;

        auto getParameter(const Str &name) const -> Pointer<const FCommandLineParameter>;

    };


}


#endif //FIELDS_INTERFACEMANAGER_H
