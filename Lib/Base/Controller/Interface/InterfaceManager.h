//
// Created by joao on 10/17/21.
//

#ifndef FIELDS_INTERFACEMANAGER_H
#define FIELDS_INTERFACEMANAGER_H


#include "Interface.h"


class InterfaceManager {
    static InterfaceManager *instance;

    std::vector<Interface*> interfaces;
public:
    static auto getInstance() -> InterfaceManager &;

    void registerInterface(Interface *anInterface);

    auto getInterfaces() -> std::vector<const Interface*>;

    void feedInterfaces(CLVariablesMap vm);

    auto renderAsPythonDictionaryEnrties() -> String;

    auto renderParametersToString(StrVector params, String separator) const -> String;

    auto getInterface(const char *string) -> const Interface*;
};


#endif //FIELDS_INTERFACEMANAGER_H
