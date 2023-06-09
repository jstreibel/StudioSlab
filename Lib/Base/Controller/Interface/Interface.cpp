//
// Created by joao on 10/13/21.
//

#include "Interface.h"
#include "InterfaceManager.h"

Interface::Interface(const String& generalDescription, bool dontRegisterInterface)
: description(generalDescription)
{
    if(!dontRegisterInterface)
        InterfaceManager::getInstance().registerInterface(me.lock());
}

auto Interface::getParameters() const -> std::vector<Parameter::ConstPtr> {
    std::vector<Parameter::ConstPtr> constParameters;

    std::copy(parameters.begin(), parameters.end(), std::back_inserter(constParameters));

    return constParameters;
}

auto Interface::getSubInterfaces() const -> std::vector<Interface::Ptr> {
    std::vector<Interface::Ptr> interfaces;

    std::copy(subInterfaces.begin(), subInterfaces.end(), std::back_inserter(interfaces));

    return interfaces;
}

void Interface::addParameter(Parameter::Ptr parameter) {
    if(!parameters.insert(parameter).second) throw "Error while inserting parameter in interface.";

    //std::cout << "Parameter \"" << parameter->getCommandLineArgName() << "\" registered to interface \""
    //          << description << "\".\n";
}

void Interface::addParameters(std::initializer_list<Parameter::Ptr> parametersList) {
    for(auto param : parametersList)
        addParameter(param);
}

void Interface::addSubInterface(Interface::Ptr subInterface) {
    if(!subInterfaces.insert(subInterface).second) throw "Error while inserting sub-interface in interface.";
}

auto Interface::getGeneralDescription() const -> String { return description; }

auto Interface::getParameter(String key) const -> Parameter::Ptr {
    auto compareFunc = [key](Parameter::Ptr parameter) {
        return parameter->operator==(key);
    };

    auto result = std::find_if(parameters.begin(), parameters.end(), compareFunc);

    return *result;
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
        for (auto param : parameters) {
            auto key = param->getCommandLineArgName(true);
            auto val = vm[key];

            param->setValueFrom(val);
        }
    } catch (boost::bad_any_cast &exception) {
        std::cout << "Exception happened in Interface \"" << getGeneralDescription() << "\"" << std::endl;
        throw exception;
    }
}

bool Interface::operator==(const Interface &rhs) const {
    return std::tie(description, parameters, subInterfaces) ==
           std::tie(rhs.description, rhs.parameters, rhs.subInterfaces);
}

bool Interface::operator==(String str) const {
    return description == str;
}

bool Interface::operator!=(const Interface &rhs) const {
    return !(rhs == *this);
}

Interface::~Interface() {

}




