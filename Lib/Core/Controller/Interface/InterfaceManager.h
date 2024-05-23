//
// Created by joao on 10/17/21.
//

#ifndef FIELDS_INTERFACEMANAGER_H
#define FIELDS_INTERFACEMANAGER_H


#include "Interface.h"


namespace Slab::Core {

    class InterfaceManager {
        static InterfaceManager *instance;

        std::vector<Interface::Ptr> interfaces;

    public:
        static auto getInstance() -> InterfaceManager &;

        // static auto NewInterface(String name, InterfaceOwner *owner) -> Interface::Ptr;

        void registerInterface(const Interface::Ptr &anInterface);

        auto getInterfaces() -> std::vector<Interface::ConstPtr>;

        auto getInterface(const char *string) -> Interface::ConstPtr;

        void feedInterfaces(const CLVariablesMap &vm);

        auto renderAsPythonDictionaryEntries() -> Str;

        auto renderParametersToString(const StrVector &params, const Str &separator = " ",
                                      bool longName = true) const -> Str;

        auto getParametersValues(const StrVector &params) const -> std::vector<std::pair<Str, Str>>;

        auto getParameter(const Str &name) const -> const Parameter &;

    };


}


#endif //FIELDS_INTERFACEMANAGER_H
