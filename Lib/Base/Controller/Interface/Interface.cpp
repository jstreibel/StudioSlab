//
// Created by joao on 10/13/21.
//

#include "Interface.h"
#include "InterfaceOwner.h"
#include "InterfaceManager.h"
#include "Common/Utils.h"
#include "Common/Log/Log.h"

Interface::Interface(String name, InterfaceOwner *owner, int priority)
    : owner(owner), priority(priority) {

    auto tokens = Common::SplitString(name, delimiter, 2);
    this->name = tokens[0];
    this->descr = tokens.size() > 1 ? tokens[1] : this->descr;

    if(owner != nullptr) addListener(owner);
    else Log::Note() << "Registering un-owned interface '" << name << "'" << Log::Flush;
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
    auto insertionSuccessful = parameters.insert(parameter).second;

    if(!insertionSuccessful){
        throw "Error while inserting parameter in interface.";
    }

    Log::Debug() << "Parameter \"" << parameter->getCommandLineArgName() << "\" registered to interface \""
                 << getName() << "\".";
}

void Interface::addParameters(std::initializer_list<Parameter::Ptr> parametersList) {


    for(auto param : parametersList)
        addParameter(param);
}

void Interface::addSubInterface(Interface::Ptr subInterface) {
    if(!subInterfaces.insert(subInterface).second) throw "Error while inserting sub-interface in interface.";
}

auto Interface::getGeneralDescription() const -> String {

    return descr!="<empty>" ? descr : "";
}

auto Interface::getParameter(String key) const -> Parameter::Ptr {
    auto compareFunc = [key](Parameter::Ptr parameter) {
        return *parameter == key;
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

        for(auto listener : listeners)
            listener->notifyCLArgsSetupFinished();

    } catch (boost::bad_any_cast &exception) {
        Log::Error() << "Exception happened in Interface \"" << getGeneralDescription() << "\"" << Log::Flush;
        throw exception;
    }
}

bool Interface::operator==(const Interface &rhs) const {
    return std::tie(name, parameters, subInterfaces) ==
           std::tie(rhs.name, rhs.parameters, rhs.subInterfaces);
}

bool Interface::operator==(String str) const {
    return name == str;
}

bool Interface::operator!=(const Interface &rhs) const {
    return !(rhs == *this);
}

Interface::~Interface() {

}

Interface::Ptr Interface::New(String name, InterfaceOwner *owner, int priority) {
    return Interface::Ptr(new Interface(name, owner, priority));
}

auto Interface::addListener(InterfaceListener *newListener) -> void {
    listeners.emplace_back(newListener);
}

auto Interface::getOwner() const -> InterfaceOwner * {
    return owner;
}

auto Interface::getName() const -> const String & {
    return name;
}

bool Interface::operator<(const Interface &other) const {
    return priority < other.priority;
}





