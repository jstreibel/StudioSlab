//
// Created by joao on 10/17/21.
//

#include "CommandLineInterfaceManager.h"

#include "Utils/Utils.h"
#include "Core/Tools/Log.h"


namespace Slab::Core {

    FCommandLineInterfaceManager *FCommandLineInterfaceManager::instance = nullptr;

    auto FCommandLineInterfaceManager::getInstance() -> FCommandLineInterfaceManager & {
        if (instance == nullptr) instance = new FCommandLineInterfaceManager;

        return *instance;
    }

    void FCommandLineInterfaceManager::registerInterface(const TPointer<FCommandLineInterface> &anInterface) {
        auto &log = Log::Note();
        log << "InterfaceManager registering interface \"" << Log::FGBlue << anInterface->GetName()
            << Log::ResetFormatting << "\" [ "
            << "priority " << anInterface->Priority << " ]";

        interfaces.emplace_back(anInterface);

        auto subInterfaces = anInterface->GetSubInterfaces();
        if (!subInterfaces.empty())
            for (const auto &subInterface: subInterfaces) {
                fix name = subInterface->GetName();
                log << "\n\t\t\t\t\t\tSub-interface: " << name;
            }

        for (const auto &p: anInterface->GetParameters()) {
            auto desc = p->getDescription();
            if (!desc.empty()) desc = " (" + desc + ")";

            log << "\n\t\t\t\t\t\tParameter: " << Log::FGBlue << p->getFullCommandLineName() << Log::ResetFormatting << desc;
        }

        log << Log::Flush;

        for (const auto &subInterface: subInterfaces)
            registerInterface(subInterface);
    }

    auto FCommandLineInterfaceManager::getInterfaces() -> Vector<TPointer<const FCommandLineInterface>> {
        Vector<TPointer<const FCommandLineInterface>> V(interfaces.size());

        std::copy(interfaces.begin(), interfaces.end(), V.begin());

        return V;
    }

    void FCommandLineInterfaceManager::feedInterfaces(const CLVariablesMap &vm) {
        Log::Debug() << "InterfaceManager started feeding interfaces." << Log::Flush;

        auto comp = [](const TPointer<FCommandLineInterface> &a, const TPointer<FCommandLineInterface> &b) { return *a < *b; };
        std::sort(interfaces.begin(), interfaces.end(), comp);

        auto &log = Log::Debug();
        log << "[priority] Interface";
        for (const auto &interface: interfaces) {

            log << "\n\t\t\t\t\t  [" << interface->Priority << "] " << interface->GetName();

            if (!interface->SubInterfaces.empty())
                log << "\t\t\t\t---> Contains " << interface->SubInterfaces.size() << " sub-interfaces.";
        }
        log << Log::Flush;

        for (const auto &interface: interfaces) {
            // TODO passar (somehow) para as interfaces somente as variaveis que importam, e não todas o tempo todo.
            // Ocorre que, passando todas sempre, certas interfaces terao acesso a informacao que nao lhes interessa.

            interface->SetupFromCommandLine(vm);
        }

        for (const auto &interface: interfaces) {
            for (auto listener: interface->Listeners)
                listener->NotifyAllCLArgsSetupFinished();
        }

        Log::Debug() << "InterfaceManager finished feeding interfaces." << Log::Flush;
    }

    auto FCommandLineInterfaceManager::renderAsPythonDictionaryEntries() -> Str {

        StringStream ss;
        for (const auto &interface: interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters)
                ss << "\"" << parameter->getCommandLineArgumentName(true) << "\": " << parameter->ValueToString() << ", ";
        }

        return ss.str();
    }

    auto FCommandLineInterfaceManager::renderParametersToString(const StrVector &params, const Str &separator,
                                                      bool longName) const -> Str {
        StringStream ss;

        for (const auto &interface: interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters) {
                auto name = parameter->getCommandLineArgumentName(longName);

                if (Contains(params, name))
                    ss << name << "=" << parameter->ValueToString() << separator;
            }
        }

        auto str = ss.str();

        return str.ends_with(separator) ? str.substr(0, str.length() - separator.length()) : str;
    }

    auto FCommandLineInterfaceManager::getInterface(const char *target) -> TPointer<const FCommandLineInterface> {
        auto compFunc = [target](const TPointer<const FCommandLineInterface> &anInterface) { return anInterface->operator==(target); };

        auto it = std::find_if(interfaces.begin(), interfaces.end(), compFunc);

        if (it == interfaces.end())
            Log::WarningImportant() << "InterfaceManager could not find Interface " << Log::FGCyan << target
                                    << Log::Flush;

        return *it;
    }

    auto FCommandLineInterfaceManager::getParametersValues(const StrVector &params) const -> Vector<Pair<Str, Str>> {
        Vector<Pair<Str, Str>> values;

        for (const auto &interface: interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters) {
                auto name = parameter->getCommandLineArgumentName();

                if (Contains(params, name))
                    values.emplace_back(name, parameter->ValueToString());
            }
        }

        return values;
    }

    auto FCommandLineInterfaceManager::getParameter(const Str &name) const -> TPointer<const FCommandLineParameter> {
        for (const auto &interface: interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters) {
                if (name == parameter->getCommandLineArgumentName() || name == parameter->getCommandLineArgumentName(true))
                    return parameter;
            }
        }

        Log::Warning() << "InterfaceManager could not find parameter '" << name << "'." << Log::Flush;
        Log::Info() << "Available parameters:" << Log::Flush;
        for (const auto &interface: interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters) {
                Log::Info() << "\t[" << interface->GetName() << "] " << parameter->getCommandLineArgumentName(true) << ": " << parameter->ValueToString() << Log::Flush;
            }
        }


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