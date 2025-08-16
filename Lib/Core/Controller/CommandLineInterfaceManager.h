//
// Created by joao on 10/17/21.
//

#ifndef FIELDS_INTERFACEMANAGER_H
#define FIELDS_INTERFACEMANAGER_H


#include "CommandLineInterface.h"


namespace Slab::Core {

    class FCommandLineInterfaceManager {
        static FCommandLineInterfaceManager *Instance;

        Vector<TPointer<FCommandLineInterface>> Interfaces;

    public:
        static auto GetInstance() -> FCommandLineInterfaceManager &;

        // static auto NewInterface(String name, InterfaceOwner *owner) -> Interface::Ptr;

        void RegisterInterface(const TPointer<FCommandLineInterface> &anInterface);

        auto GetInterfaces() -> Vector<TPointer<const FCommandLineInterface>>;

        auto GetInterface(const char *string) -> TPointer<const FCommandLineInterface>;

        void FeedInterfaces(const CLVariablesMap &vm);

        auto RenderAsPythonDictionaryEntries() -> Str;

        [[nodiscard]] auto RenderParametersToString(const StrVector &params, const Str &separator = " ",
                                      bool longName = true) const -> Str;

        [[nodiscard]] auto GetParametersValues(const StrVector &params) const -> Vector<Pair<Str, Str>>;

        [[nodiscard]] auto GetParameter(const Str &name) const -> TPointer<const FCommandLineParameter>;

    };


}


#endif //FIELDS_INTERFACEMANAGER_H
