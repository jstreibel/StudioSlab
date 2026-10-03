//
// Created by joao on 10/17/21.
//

#include "InterfaceManager.h"

#include "Utils/Utils.h"
#include "Core/Tools/Log.h"

namespace {
    auto EscapePythonString(const Slab::Str &value) -> Slab::Str {
        Slab::Str escaped;
        escaped.reserve(value.size());

        for (const auto character: value) {
            switch (character) {
                case '\\':
                    escaped += "\\\\";
                    break;
                case '"':
                    escaped += "\\\"";
                    break;
                case '\n':
                    escaped += "\\n";
                    break;
                case '\r':
                    escaped += "\\r";
                    break;
                case '\t':
                    escaped += "\\t";
                    break;
                default:
                    escaped += character;
                    break;
            }
        }

        return escaped;
    }
}


namespace Slab::Core {

    FInterfaceManager *FInterfaceManager::Instance = nullptr;

    auto FInterfaceManager::GetInstance() -> FInterfaceManager & {
        if (Instance == nullptr) Instance = new FInterfaceManager;

        return *Instance;
    }

    void FInterfaceManager::RegisterInterface(const TPointer<FInterface> &anInterface) {
        auto &log = FLog::Note();
        log << "InterfaceManager registering interface \"" << FLog::FGBlue << anInterface->GetName()
            << FLog::ResetFormatting << "\" [ "
            << "priority " << anInterface->Priority << " ]";

        Interfaces.emplace_back(anInterface);

        auto subInterfaces = anInterface->GetSubInterfaces();
        if (!subInterfaces.empty())
            for (const auto &subInterface: subInterfaces) {
                fix name = subInterface->GetName();
                log << "\n\t\t\t\t\t\tSub-interface: " << name;
            }

        for (const auto &p: anInterface->GetParameters()) {
            auto desc = p->GetDescription();
            if (!desc.empty()) desc = " (" + desc + ")";

            log << "\n\t\t\t\t\t\tParameter: " << FLog::FGBlue << p->GetFullCommandLineName() << FLog::ResetFormatting << desc;
        }

        log << FLog::Flush;

        for (const auto &subInterface: subInterfaces)
            RegisterInterface(subInterface);
    }

    auto FInterfaceManager::GetInterfaces() -> Vector<TPointer<const FInterface>> {
        Vector<TPointer<const FInterface>> V(Interfaces.size());

        std::copy(Interfaces.begin(), Interfaces.end(), V.begin());

        return V;
    }

    void FInterfaceManager::FeedInterfaces(const CLVariablesMap &vm) {
        FLog::Debug() << "InterfaceManager started feeding interfaces." << FLog::Flush;

        auto comp = [](const TPointer<FInterface> &a, const TPointer<FInterface> &b) { return *a < *b; };
        std::sort(Interfaces.begin(), Interfaces.end(), comp);
        auto interfaces = Interfaces;

        auto &log = FLog::Debug();
        log << "[priority] Interface";
        for (const auto &interface: interfaces) {

            log << "\n\t\t\t\t\t  [" << interface->Priority << "] " << interface->GetName();

            if (!interface->SubInterfaces.empty())
                log << "\t\t\t\t---> Contains " << interface->SubInterfaces.size() << " sub-interfaces.";
        }
        log << FLog::Flush;

        for (const auto &interface: interfaces) {
            // TODO passar (somehow) para as interfaces somente as variaveis que importam, e não todas o tempo todo.
            // Ocorre que, passando todas sempre, certas interfaces terao acesso a informacao que nao lhes interessa.

            interface->SetupFromCommandLine(vm);
        }

        // All-interface callbacks may start backends and register additional
        // interfaces. Iterate the same stable batch used above so those
        // registrations cannot invalidate this loop.
        for (const auto &Interface: interfaces) {
            for (auto Listener: Interface->Listeners)
                Listener->SendMessage(FPayload::AllCommandLineParsingFinished);
                // listener->NotifyAllCLArgsSetupFinished();
        }

        FLog::Debug() << "InterfaceManager finished feeding interfaces." << FLog::Flush;
    }

    auto FInterfaceManager::RenderAsPythonDictionaryEntries() -> Str {

        StringStream ss;
        for (const auto &interface: Interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters) {
                ss << "\"" << parameter->GetCommandLineArgumentName(true) << "\": ";

                const auto type = parameter->GetType();
                if (type == EParameterType::ParameterType_String || type == EParameterType::ParameterType_MultiString)
                    ss << "\"" << EscapePythonString(parameter->ValueToString()) << "\"";
                else
                    ss << parameter->ValueToString();

                ss << ", ";
            }
        }

        return ss.str();
    }

    auto FInterfaceManager::RenderParametersToString(const StrVector &params, const Str &separator,
                                                      bool longName) const -> Str {
        StringStream ss;

        for (const auto &interface: Interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters) {
                auto name = parameter->GetCommandLineArgumentName(longName);

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
            FLog::WarningImportant() << "InterfaceManager could not find Interface " << FLog::FGCyan << target
                                    << FLog::Flush;

        return *it;
    }

    auto FInterfaceManager::GetParametersValues(const StrVector &params) const -> Vector<Pair<Str, Str>> {
        Vector<Pair<Str, Str>> values;

        for (const auto &interface: Interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters) {
                auto name = parameter->GetCommandLineArgumentName();

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
                if (name == parameter->GetCommandLineArgumentName() || name == parameter->GetCommandLineArgumentName(true))
                    return parameter;
            }
        }

        FLog::Warning() << "InterfaceManager could not find parameter '" << name << "'." << FLog::Flush;
        FLog::Info() << "Available parameters:" << FLog::Flush;
        for (const auto &interface: Interfaces) {
            auto parameters = interface->GetParameters();
            for (const auto &parameter: parameters) {
                FLog::Info() << "\t[" << interface->GetName() << "] " << parameter->GetCommandLineArgumentName(true) << ": " << parameter->ValueToString() << FLog::Flush;
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