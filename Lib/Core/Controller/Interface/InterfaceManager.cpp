//
// Created by joao on 10/17/21.
//

#include "InterfaceManager.h"

#include "Utils/Utils.h"
#include "Core/Tools/Log.h"


namespace Slab::Core {

    InterfaceManager *InterfaceManager::instance = nullptr;

    auto InterfaceManager::getInstance() -> InterfaceManager & {
        if (instance == nullptr) instance = new InterfaceManager;

        return *instance;
    }

    void InterfaceManager::registerInterface(const Pointer<Interface> &anInterface) {
        auto &log = Log::Note();
        log << "InterfaceManager registering interface \"" << Log::FGBlue << anInterface->getName()
            << Log::ResetFormatting << "\" [ "
            << "priority " << anInterface->priority << " ]";

        interfaces.emplace_back(anInterface);

        auto subInterfaces = anInterface->getSubInterfaces();
        if (!subInterfaces.empty())
            for (const auto &subInterface: subInterfaces)
                log << "\n\t\t\t\t\t\tSub-interface: " << subInterface->getName();

        for (const auto &p: anInterface->getParameters()) {
            auto desc = p->getDescription();
            if (!desc.empty()) desc = " (" + desc + ")";

            log << "\n\t\t\t\t\t\tParameter: " << Log::FGBlue << p->getFullCLName() << Log::ResetFormatting << desc;
        }

        log << Log::Flush;

        for (const auto &subInterface: subInterfaces)
            registerInterface(subInterface);
    }

    auto InterfaceManager::getInterfaces() -> Vector<Pointer<const Interface>> {
        Vector<Pointer<const Interface>> V(interfaces.size());

        std::copy(interfaces.begin(), interfaces.end(), V.begin());

        return V;
    }

    void InterfaceManager::feedInterfaces(const CLVariablesMap &vm) {
        Log::Critical() << "InterfaceManager started feeding interfaces." << Log::Flush;

        auto comp = [](const Interface_ptr &a, const Interface_ptr &b) { return *a < *b; };
        std::sort(interfaces.begin(), interfaces.end(), comp);

        auto &log = Log::Info();
        log << "[priority] Interface";
        for (const auto &interface: interfaces) {

            log << "\n\t\t\t\t\t  [" << interface->priority << "] " << interface->getName();

            if (!interface->subInterfaces.empty())
                log << "\t\t\t\t---> Contains " << interface->subInterfaces.size() << " sub-interfaces.";
        }
        log << Log::Flush;

        for (const auto &interface: interfaces) {
            // TODO passar (somehow) para as interfaces somente as variaveis que importam, e não todas o tempo todo.
            // Ocorre que, passando todas sempre, certas interfaces terao acesso a informacao que nao lhes interessa.

            interface->setup(vm);
        }

        for (const auto &interface: interfaces) {
            for (auto listener: interface->listeners)
                listener->notifyAllCLArgsSetupFinished();
        }

        Log::Success() << "InterfaceManager finished feeding interfaces." << Log::Flush;
    }

    auto InterfaceManager::renderAsPythonDictionaryEntries() -> Str {

        StringStream ss;
        for (const auto &interface: interfaces) {
            auto parameters = interface->getParameters();
            for (const auto &parameter: parameters)
                ss << "\"" << parameter->getCLName(true) << "\": " << parameter->valueToString() << ", ";
        }

        return ss.str();
    }

    auto InterfaceManager::renderParametersToString(const StrVector &params, const Str &separator,
                                                    bool longName) const -> Str {
        StringStream ss;

        for (const auto &interface: interfaces) {
            auto parameters = interface->getParameters();
            for (const auto &parameter: parameters) {
                auto name = parameter->getCLName(longName);

                if (Common::Contains(params, name))
                    ss << name << "=" << parameter->valueToString() << separator;
            }
        }

        auto str = ss.str();

        return str.ends_with(separator) ? str.substr(0, str.length() - separator.length()) : str;
    }

    auto InterfaceManager::getInterface(const char *target) -> Interface_constptr {
        auto compFunc = [target](const Interface_constptr &anInterface) { return anInterface->operator==(target); };

        auto it = std::find_if(interfaces.begin(), interfaces.end(), compFunc);

        if (it == interfaces.end())
            Log::WarningImportant() << "InterfaceManager could not find Interface " << Log::FGCyan << target
                                    << Log::Flush;

        return *it;
    }

    auto InterfaceManager::getParametersValues(const StrVector &params) const -> Vector<Pair<Str, Str>> {
        Vector<Pair<Str, Str>> values;

        for (const auto &interface: interfaces) {
            auto parameters = interface->getParameters();
            for (const auto &parameter: parameters) {
                auto name = parameter->getCLName();

                if (Common::Contains(params, name))
                    values.emplace_back(name, parameter->valueToString());
            }
        }

        return values;
    }

    auto InterfaceManager::getParameter(const Str &name) const -> Pointer<const Parameter> {
        for (const auto &interface: interfaces) {
            auto parameters = interface->getParameters();
            for (const auto &parameter: parameters) {
                if (name == parameter->getCLName() || name == parameter->getCLName(true))
                    return parameter;
            }
        }

        Log::Warning() << "InterfaceManager could not find parameter '" << name << "'." << Log::Flush;

        return nullptr;
    }

//auto InterfaceManager::NewInterface(String name, InterfaceOwner *owner) -> Interface::Ptr {
//    auto newInterface = Interface::Ptr(new Interface(name, owner));
//
//    getInstance().registerInterface(newInterface);
//
//    return newInterface;
//}


}