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

    void registerInterface(Interface::Ptr anInterface);

    auto getInterfaces() -> std::vector<Interface::ConstPtr>;

    void feedInterfaces(CLVariablesMap vm);

    auto renderAsPythonDictionaryEntries() -> String;

    auto renderParametersToString(StrVector params, String separator) const -> String;

    auto getInterface(const char *string) -> Interface::ConstPtr;
};


#endif //FIELDS_INTERFACEMANAGER_H
