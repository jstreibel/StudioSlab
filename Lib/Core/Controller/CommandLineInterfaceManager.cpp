//
// Created by joao on 10/17/21.
//

#include "CommandLineInterfaceManager.h"

#include "Utils/Utils.h"
#include "Core/Tools/Log.h"


namespace Slab::Core {

    FCommandLineInterfaceManager *FCommandLineInterfaceManager::Instance = nullptr;

    auto FCommandLineInterfaceManager::GetInstance() -> FCommandLineInterfaceManager & {
        if (Instance == nullptr) Instance = new FCommandLineInterfaceManager;

        return *Instance;
    }

    void FCommandLineInterfaceManager::RegisterInterface(const TPointer<FCommandLineInterface> &anInterface) {
        auto &log = Log::Note();
        log << "InterfaceManager registering interface \"" << Log::FGBlue << anInterface->GetName()
            << Log::ResetFormatting << "\" [ "
            << "priority " << anInterface->Priority << " ]";

        Interfaces.emplace_back(anInterface);

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
            RegisterInterface(subInterface);
    }

    auto FCommandLineInterfaceManager::GetInterfaces() -> Vector<TPointer<const FCommandLineInterface>> {
        Vector<TPointer<const FCommandLineInterface>> V(Interfaces.size());

        std::copy(Interfaces.begin(), Interfaces.end(), V.begin());

        return V;
    }

    void FCommandLineInterfaceManager::FeedInterfaces(const CLVariablesMap &vm) {
        Log::Debug() << "InterfaceManager started feeding interfaces." << Log::Flush;

        auto comp = [](const TPointer<FCommandLineInterface> &a, const TPointer<FCommandLineInterface> &b) { return *a < *b; };
        std::sort(Interfaces.begin(), Interfaces.end(), comp);

        auto &log = Log::Debug();
        log << "[priority] Interface";
        for (const auto &interface: Interfaces) {

            log << "\n\t\t\t\t\t  [" << interface->Priority << "] " << interface->GetName();

            if (!interface->SubInterfaces.empty())
                log << "\t\t\t\t---> Contains " << interface->SubInterfaces.size() << " sub-interfaces.";
        }
        log << Log::Flush;

        for (const auto &interface: Interfaces) {
            // TODO passar (somehow) para as interfaces somente as variaveis que importam, e não todas o tempo todo.
            // Ocorre que, passando todas sempre, certas interfaces terao acesso a informacao que nao lhes interessa.

            interface->SetupFromCommandLine(vm);
        }

        for (const auto &Interface: Interfaces) {
            for (auto Listener: Interface->Listeners)
                Listener->SendMessage(FPayload::AllCommandLineParsingFinished);
                // listener->NotifyAllCLArgsSetupFinished();
        }

        Log::Debug() << "InterfaceManager finished feeding interfaces." << Log::Flush;
    }

    auto FCommandLineInterfaceManager::RenderAsPythonDictionaryEntries() -> Str {

        StringStream ss;
        for (const auto &interface: Interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters)
                ss << "\"" << parameter->getCommandLineArgumentName(true) << "\": " << parameter->ValueToString() << ", ";
        }

        return ss.str();
    }

    auto FCommandLineInterfaceManager::RenderParametersToString(const StrVector &params, const Str &separator,
                                                      bool longName) const -> Str {
        StringStream ss;

        for (const auto &interface: Interfaces) {
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

    auto FCommandLineInterfaceManager::GetInterface(const char *target) -> TPointer<const FCommandLineInterface> {
        auto compFunc = [target](const TPointer<const FCommandLineInterface> &anInterface) { return anInterface->operator==(target); };

        auto it = std::find_if(Interfaces.begin(), Interfaces.end(), compFunc);

        if (it == Interfaces.end())
            Log::WarningImportant() << "InterfaceManager could not find Interface " << Log::FGCyan << target
                                    << Log::Flush;

        return *it;
    }

    auto FCommandLineInterfaceManager::GetParametersValues(const StrVector &params) const -> Vector<Pair<Str, Str>> {
        Vector<Pair<Str, Str>> values;

        for (const auto &interface: Interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters) {
                auto name = parameter->getCommandLineArgumentName();

                if (Contains(params, name))
                    values.emplace_back(name, parameter->ValueToString());
            }
        }

        return values;
    }

    auto FCommandLineInterfaceManager::GetParameter(const Str &name) const -> TPointer<const FCommandLineParameter> {
        for (const auto &interface: Interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters) {
                if (name == parameter->getCommandLineArgumentName() || name == parameter->getCommandLineArgumentName(true))
                    return parameter;
            }
        }

        Log::Warning() << "InterfaceManager could not find parameter '" << name << "'." << Log::Flush;
        Log::Info() << "Available parameters:" << Log::Flush;
        for (const auto &interface: Interfaces) {
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