//
// Created by joao on 10/13/21.
//

#include "Interface.h"
#include "InterfaceManager.h"

Interface::Interface(/*std::initializer_list<Parameter*> parametersList, */const String& generalDescription, bool doNotRegisterInterface)
: description(generalDescription)
{
    if(!doNotRegisterInterface)
        InterfaceManager::getInstance().registerInterface(this);
}

auto Interface::getParameters() const -> std::vector<const Parameter *> {
    std::vector<const Parameter *> constParameters;

    std::copy(parameters.begin(), parameters.end(), std::back_inserter(constParameters));

    return constParameters;
}

void Interface::addParameter(Parameter *parameter) {
    if(!parameters.insert(parameter).second) throw "Error while inserting parameter in interface.";

    //std::cout << "Parameter \"" << parameter->getCommandLineArgName() << "\" registered to interface \""
    //          << description << "\".\n";
}

void Interface::addParameters(std::initializer_list<Parameter*> parametersList) {
    for(auto *param : parametersList)
        addParameter(param);
}

auto Interface::getGeneralDescription() const -> String { return description; }

void Interface::setup(CLVariablesMap vm) {
    for(auto *param : parameters) {
        auto key = param->getCommandLineArgName();
        auto val = vm[key];

        param->setValueFrom(val);
    }
}

auto Interface::getParameter(String key) const -> Parameter * {
    return nullptr;
}
