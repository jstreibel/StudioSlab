//
// Created by joao on 10/13/21.
//

#include "Interface.h"
#include "InterfaceOwner.h"
#include "InterfaceManager.h"
#include "Common/Utils.h"
#include "Base/Tools/Log.h"

Interface::Interface(Str name, InterfaceOwner *owner, int priority)
    : owner(owner), priority(priority) {

    auto tokens = Common::SplitString(name, delimiter, 2);
    this->name = tokens[0];
    this->descr = tokens.size() > 1 ? tokens[1] : this->descr;

    if(owner != nullptr) addListener(owner);

    Log::Info() << "Interface '" << Log::FGGreen << name << Log::ResetFormatting << "' created. " << Log::Flush;
    if(owner == nullptr)
        Log::Attention()  << "Interface '" << Log::FGGreen << name << Log::ResetFormatting << "' is NOT owned." << Log::Flush;
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

    auto name = Str("\"") + parameter->getFullCLName() + "\"";
    Log::Note() << "Parameter " << std::setw(25) << std::left << name << " registered to interface \"" << getName() << "\".";
}

void Interface::addParameters(std::initializer_list<Parameter::Ptr> parametersList) {
    for(auto param : parametersList)
        addParameter(param);
}

void Interface::addParameters(std::initializer_list<Parameter *> parametersList) {
    for(auto param : parametersList)
        addParameter(DummyPtr(*param));
}


void Interface::addSubInterface(Interface::Ptr subInterface) {
    if(Common::Contains(subInterfaces, subInterface))
        throw Str("Error while inserting sub-interface '") + subInterface->getName()
            + Str("' in interface '") + this->getName() + Str("': interface contains sub interface already");

    if(!subInterfaces.insert(subInterface).second){
        throw Str("Error while inserting sub-interface '") + subInterface->getName()
              + Str("' in interface '") + this->getName() + Str("': not specified");
    }
}

auto Interface::getGeneralDescription() const -> Str {

    return descr!="<empty>" ? descr : "";
}

auto Interface::getParameter(Str key) const -> Parameter::Ptr {
    auto compareFunc = [key](Parameter::Ptr parameter) {
        return *parameter == key;
    };

    auto result = std::find_if(parameters.begin(), parameters.end(), compareFunc);

    return *result;
}

auto Interface::toString(StrVector paramNames, Str separator, bool longName) const -> Str {
    std::stringstream ss;

    for(auto param : parameters) {
        auto nameShort = param->getCLName(false);
        auto nameLong  = param->getCLName(true);

        if(Common::Contains(paramNames, nameShort) || Common::Contains(paramNames, nameLong) || paramNames.empty())
            ss << param->getCLName(longName) << "=" << param->valueToString() << separator;
    }

    auto str = ss.str();
    if(!parameters.empty())
        for(int i=0; i<separator.size(); i++) str.pop_back(); // remove trailing sparator

    return str;
}

void Interface::setup(CLVariablesMap vm) {
    try {
        for (auto param : parameters) {
            auto key = param->getCLName(true);
            auto val = vm[key];

            param->setValueFrom(val);
        }

        for(auto listener : listeners)
            listener->notifyCLArgsSetupFinished();

    } catch (cxxopts::exceptions::exception &exception) {
        Log::Error() << "Exception happened in Interface \"" << getGeneralDescription() << "\"" << Log::Flush;
        throw exception;
    }
}

bool Interface::operator==(const Interface &rhs) const {
    return std::tie(name, parameters, subInterfaces) ==
           std::tie(rhs.name, rhs.parameters, rhs.subInterfaces);
}

bool Interface::operator==(Str str) const {
    return name == str;
}

bool Interface::operator!=(const Interface &rhs) const {
    return !(rhs == *this);
}

Interface::~Interface() {

}

Interface::Ptr Interface::New(Str name, InterfaceOwner *owner, int priority) {
    return Interface::Ptr(new Interface(name, owner, priority));
}

auto Interface::addListener(InterfaceListener *newListener) -> void {
    listeners.emplace_back(newListener);
}

auto Interface::getOwner() const -> InterfaceOwner * {
    return owner;
}

auto Interface::getName() const -> const Str & {
    return name;
}

bool Interface::operator<(const Interface &other) const {
    return priority < other.priority;
}






