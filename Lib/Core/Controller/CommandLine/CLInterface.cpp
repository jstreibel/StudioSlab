//
// Created by joao on 10/13/21.
//

#include "CLInterface.h"
#include "CLInterfaceOwner.h"
#include "CLInterfaceManager.h"
#include "Utils/Utils.h"
#include "Core/Tools/Log.h"

#include <iomanip>


namespace Slab::Core {

    CLInterface::CLInterface(Str name, CLInterfaceOwner *owner, int priority)
            : owner(owner), priority(priority), protocols(owner->getProtocols()) {


        auto tokens = Common::SplitString(name, delimiter, 2);
        this->name = tokens[0];
        this->descr = tokens.size() > 1 ? tokens[1] : this->descr;

        if (owner != nullptr) addListener(owner);

        Log::Status() << "Interface '" << Log::FGGreen << name << Log::ResetFormatting << "' created. " << Log::Flush;
        if (owner == nullptr) {
            Log::Attention() << "Interface '" << Log::FGGreen << name << Log::ResetFormatting << "' is NOT owned."
                             << Log::Flush;

            NOT_IMPLEMENTED
        }
    }

    auto CLInterface::getParameters() const -> Vector<CLParameter_constptr> {
        Vector<CLParameter_constptr> constParameters;

        std::copy(parameters.begin(), parameters.end(), std::back_inserter(constParameters));

        return constParameters;
    }

    auto CLInterface::getSubInterfaces() const -> Vector<Pointer<CLInterface>> {
        Vector<Pointer<CLInterface>> interfaces;

        std::copy(subInterfaces.begin(), subInterfaces.end(), std::back_inserter(interfaces));

        return interfaces;
    }

    void CLInterface::addParameter(CLParameter_ptr parameter) {
        auto insertionSuccessful = parameters.insert(parameter).second;

        if (!insertionSuccessful) {
            throw "Error while inserting parameter in interface.";
        }

        auto quotename = Str("\"") + parameter->getFullCommandLineName() + "\"";
        Log::Note() << "Parameter " << std::setw(25) << std::left << quotename << " registered to interface \"" << getName()
                    << "\".";
    }

    void CLInterface::addParameters(std::initializer_list<CLParameter_ptr> parametersList) {
        for (auto param: parametersList)
            addParameter(param);
    }

    void CLInterface::addParameters(std::initializer_list<CLParameter *> parametersList) {
        for (auto param: parametersList)
            addParameter(Naked(*param));
    }


    void CLInterface::addSubInterface(const Pointer<CLInterface>& subInterface) {
        if (Contains(subInterfaces, subInterface))
            throw Str("Error while inserting sub-interface '") + subInterface->getName()
                  + Str("' in interface '") + this->getName() + Str("': interface contains sub interface already");

        if (!subInterfaces.insert(subInterface).second) {
            throw Str("Error while inserting sub-interface '") + subInterface->getName()
                  + Str("' in interface '") + this->getName() + Str("': not specified");
        }
    }

    auto CLInterface::getGeneralDescription() const -> Str {

        return descr != "<empty>" ? descr : "";
    }

    auto CLInterface::getParameter(Str key) const -> CLParameter_ptr {
        auto compareFunc = [key](CLParameter_ptr parameter) {
            return *parameter == key;
        };

        auto result = std::find_if(parameters.begin(), parameters.end(), compareFunc);

        return *result;
    }

    auto CLInterface::toString(const StrVector &paramNames, Str separator, bool longName) const -> Str {
        std::stringstream ss("");

        std::map<Str, int> paramCount;
        for (auto &p: paramNames) paramCount[p] = 0;

        fix LONG_NAME = true;
        fix SHORT_NAME = false;

        for (auto &param: parameters) {
            auto nameShort = param->getCommandLineArgumentName(SHORT_NAME);
            auto nameLong = param->getCommandLineArgumentName(LONG_NAME);

            if (Contains(paramNames, nameShort) || Contains(paramNames, nameLong) ||
                paramNames.empty()) {
                bool isLong = !nameLong.empty();
                ss << param->getCommandLineArgumentName(isLong) << "=" << param->valueToString() << separator;

                paramCount[isLong ? nameLong : nameShort]++;
            }
        }

        for (auto &pCount: paramCount)
            if (pCount.second == 0)
                Log::Warning() << __PRETTY_FUNCTION__ << " could not find parameter " << pCount.first << Log::Flush;

        auto str = ss.str();
        if (!parameters.empty())
            for (int i = 0; i < separator.size(); i++) str.pop_back(); // remove trailing sparator

        return str;
    }

    void CLInterface::setupFromCommandLine(CLVariablesMap vm) {
        try {
            for (auto param: parameters) {
                auto key = param->getCommandLineArgumentName(true);
                auto val = vm[key];

                param->setValueFromCommandLine(val);
            }

            for (auto listener: listeners)
                listener->notifyCLArgsSetupFinished();

        } catch (cxxopts::exceptions::exception &exception) {
            Log::Error() << "Exception happened in Interface \"" << getGeneralDescription() << "\"" << Log::Flush;
            throw exception;
        }
    }

    bool CLInterface::operator==(const CLInterface &rhs) const {
        return std::tie(name, parameters, subInterfaces) ==
               std::tie(rhs.name, rhs.parameters, rhs.subInterfaces);
    }

    bool CLInterface::operator==(Str str) const {
        return name == str;
    }

    bool CLInterface::operator!=(const CLInterface &rhs) const {
        return !(rhs == *this);
    }

    CLInterface::~CLInterface() {

    }

    auto CLInterface::addListener(CLInterfaceListener *newListener) -> void {
        listeners.emplace_back(newListener);
    }

    auto CLInterface::getOwner() const -> CLInterfaceOwner * {
        return owner;
    }

    auto CLInterface::getName() const -> const Str & {
        return name;
    }

    bool CLInterface::operator<(const CLInterface &other) const {
        return priority < other.priority;
    }

    Message CLInterface::sendRequest(Request req) {
        if(!Contains(protocols, req)) {
            return {"[unknown request]"};
        }

        return owner->requestIssued(req);
    }


}
