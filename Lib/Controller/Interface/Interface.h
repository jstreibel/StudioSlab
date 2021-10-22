//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_INTERFACE_H
#define FIELDS_INTERFACE_H


#include "ParameterUtils.h"

#include <set>

class InterfaceManager;

class Interface {
protected:
    friend InterfaceManager;

    std::set<Parameter*> parameters;
    void addParameter(Parameter *parameter);
    void addParameters(std::initializer_list<Parameter*> parameters);

    String description;

public:
    Interface(/*std::initializer_list<Parameter*> parameters, */const String& generalDescription, bool doNotRegisterInterface=false);

    auto getParameters() const -> std::vector<const Parameter*>;
    auto getParameter(String key) const -> Parameter*;

    auto getGeneralDescription() const -> String;

    virtual void setup(CLVariablesMap vm);
};


#endif //FIELDS_INTERFACE_H
