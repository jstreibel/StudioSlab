//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_INTERFACE_H
#define FIELDS_INTERFACE_H

#include "Parameter.h"

#include <set>

class InterfaceManager;

class Interface {
protected:
    friend InterfaceManager;
    String description;

    std::set<Parameter*> parameters;
    void addParameter(Parameter *parameter);
    void addParameters(std::initializer_list<Parameter*> parameters);

    std::set<Interface*> subInterfaces;
    void addSubInterface(Interface *subInterface);

public:
    Interface(/*std::initializer_list<Parameter*> parameters, */const String& generalDescription, bool doNotRegisterInterface=false);

    auto getGeneralDescription() const -> String;

    auto getParameters() const -> std::vector<const Parameter*>;
    auto getParameter(String key) const -> Parameter*;

    auto getSubInterfaces() const -> std::vector<Interface*>;

    virtual auto toString() const -> String;
    virtual void setup(CLVariablesMap vm);
};


#endif //FIELDS_INTERFACE_H
