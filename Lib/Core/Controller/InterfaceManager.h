//
// Created by joao on 10/17/21.
//

#ifndef FIELDS_INTERFACEMANAGER_H
#define FIELDS_INTERFACEMANAGER_H


#include "Interface.h"


namespace Slab::Core {

    class FInterfaceManager {
        static FInterfaceManager *Instance;

        Vector<TPointer<FInterface>> Interfaces;

    public:
        static auto GetInstance() -> FInterfaceManager &;

        // static auto NewInterface(String name, InterfaceOwner *owner) -> Interface::Ptr;

        void RegisterInterface(const TPointer<FInterface> &anInterface);

        auto GetInterfaces() -> Vector<TPointer<const FInterface>>;

        auto GetInterface(const char *string) -> TPointer<const FInterface>;

        void FeedInterfaces(const CLVariablesMap &vm);

        auto RenderAsPythonDictionaryEntries() -> Str;

        [[nodiscard]] auto RenderParametersToString(const StrVector &params, const Str &separator = " ",
                                      bool longName = true) const -> Str;

        [[nodiscard]] auto GetParametersValues(const StrVector &params) const -> Vector<Pair<Str, Str>>;

        [[nodiscard]] auto GetParameter(const Str &name) const -> TPointer<const FParameter>;

    };


}


#endif //FIELDS_INTERFACEMANAGER_H
