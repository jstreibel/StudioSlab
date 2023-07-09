//
// Created by joao on 10/17/21.
//

#ifndef FIELDS_INTERFACEMANAGER_H
#define FIELDS_INTERFACEMANAGER_H


#include "Interface.h"


class InterfaceManager {
    static InterfaceManager *instance;

    std::vector<Interface::Ptr> interfaces;

public:
    static auto getInstance() -> InterfaceManager &;

    // static auto NewInterface(String name, InterfaceOwner *owner) -> Interface::Ptr;

    void registerInterface(Interface::Ptr anInterface);
    auto getInterfaces() -> std::vector<Interface::ConstPtr>;
    auto getInterface(const char *string) -> Interface::ConstPtr;

    void feedInterfaces(CLVariablesMap vm);

    auto renderAsPythonDictionaryEntries() -> Str;

    auto renderParametersToString(StrVector params, Str separator) const -> Str;

};


#endif //FIELDS_INTERFACEMANAGER_H
