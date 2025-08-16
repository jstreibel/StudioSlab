//
// Created by joao on 10/17/21.
//

#include "InterfaceManager.h"

#include "Utils/Utils.h"
#include "Core/Tools/Log.h"


namespace Slab::Core {

    FInterfaceManager *FInterfaceManager::Instance = nullptr;

    auto FInterfaceManager::GetInstance() -> FInterfaceManager & {
        if (Instance == nullptr) Instance = new FInterfaceManager;

        return *Instance;
    }

    void FInterfaceManager::RegisterInterface(const TPointer<FInterface> &anInterface) {
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

    auto FInterfaceManager::GetInterfaces() -> Vector<TPointer<const FInterface>> {
        Vector<TPointer<const FInterface>> V(Interfaces.size());

        std::copy(Interfaces.begin(), Interfaces.end(), V.begin());

        return V;
    }

    void FInterfaceManager::FeedInterfaces(const CLVariablesMap &vm) {
        Log::Debug() << "InterfaceManager started feeding interfaces." << Log::Flush;

        auto comp = [](const TPointer<FInterface> &a, const TPointer<FInterface> &b) { return *a < *b; };
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

    auto FInterfaceManager::RenderAsPythonDictionaryEntries() -> Str {

        StringStream ss;
        for (const auto &interface: Interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters)
                ss << "\"" << parameter->getCommandLineArgumentName(true) << "\": " << parameter->ValueToString() << ", ";
        }

        return ss.str();
    }

    auto FInterfaceManager::RenderParametersToString(const StrVector &params, const Str &separator,
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

    auto FInterfaceManager::GetInterface(const char *target) -> TPointer<const FInterface> {
        auto compFunc = [target](const TPointer<const FInterface> &anInterface) { return anInterface->operator==(target); };

        auto it = std::find_if(Interfaces.begin(), Interfaces.end(), compFunc);

        if (it == Interfaces.end())
            Log::WarningImportant() << "InterfaceManager could not find Interface " << Log::FGCyan << target
                                    << Log::Flush;

        return *it;
    }

    auto FInterfaceManager::GetParametersValues(const StrVector &params) const -> Vector<Pair<Str, Str>> {
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

    auto FInterfaceManager::GetParameter(const Str &name) const -> TPointer<const FParameter> {
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