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

auto Interface::getSubInterfaces() const -> std::vector<Interface*> {
    std::vector<Interface*> interfaces;

    std::copy(subInterfaces.begin(), subInterfaces.end(), std::back_inserter(interfaces));

    return interfaces;
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

void Interface::addSubInterface(Interface *subInterface) {
    if(!subInterfaces.insert(subInterface).second) throw "Error while inserting sub-interface in interface.";
}

auto Interface::getGeneralDescription() const -> String { return description; }

auto Interface::getParameter(String key) const -> Parameter * {
    throw "Not implemented";
    //return parameters[key];
}

auto Interface::toString() const -> String {
    std::stringstream ss;

    for(auto param : parameters)
        ss << param->getCommandLineArgName(true) << "=" << param->valueToString() << "-";

    auto str = ss.str();
    if(!parameters.empty())
        str.pop_back(); // remove trailing '-'

    return str;
}

void Interface::setup(CLVariablesMap vm) {
    try {
        for (auto *param : parameters) {
            auto key = param->getCommandLineArgName(true);
            auto val = vm[key];

            param->setValueFrom(val);
        }
    } catch (boost::bad_any_cast &exception) {
        std::cout << "Exception happened in Interface \"" << getGeneralDescription() << "\"" << std::endl;
        throw exception;
    }
}




