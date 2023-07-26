//
// Created by joao on 10/17/21.
//

#include "InterfaceManager.h"

#include "Common/Utils.h"
#include "Common/Log/Log.h"

InterfaceManager *InterfaceManager::instance = nullptr;

auto InterfaceManager::getInstance() -> InterfaceManager & {
    if(instance == nullptr) instance = new InterfaceManager;

    return *instance;
}

void InterfaceManager::registerInterface(Interface::Ptr anInterface) {
    auto size = anInterface->parameters.size();

    Log::Info() << "InterfaceManager registering interface \"" << anInterface->getName() << "\" [ "
                << "priority " << anInterface->priority << " ]" << Log::Flush;

    interfaces.emplace_back(anInterface);

    auto subInterfaces = anInterface->getSubInterfaces();
    for (auto subInterface: subInterfaces)
        registerInterface(subInterface);
}

auto InterfaceManager::getInterfaces() -> std::vector<Interface::ConstPtr> {
    std::vector<Interface::ConstPtr> V(interfaces.size());

    std::copy(interfaces.begin(), interfaces.end(), V.begin());

    return V;
}

void InterfaceManager::feedInterfaces(CLVariablesMap vm) {
    Log::Attention() << "InterfaceManager started feeding interfaces." << Log::Flush;

    auto comp = [](const Interface::Ptr& a, const Interface::Ptr& b) { return *a < *b; };
    std::sort(interfaces.begin(), interfaces.end(), comp);

    auto &log = Log::Debug();
    log << "[priority] Interface";
    for(auto interface : interfaces){

        log << "\n\t\t\t[" << interface->priority << "] " << interface->getName();

        if(!interface->subInterfaces.empty())
            log << "\t\t---> Contains " << interface->subInterfaces.size() << " sub-interfaces.";
    }
    log << Log::Flush;

    for(auto interface : interfaces){
        // TODO passar (somehow) para as interfaces somente as variaveis que importam, e não todas o tempo todo.
        // Ocorre que, passando todas sempre, certas interfaces terao acesso a informacao que nao lhes interessa.

        interface->setup(vm);
    }

    Log::Info() << "InterfaceManager finished feeding interfaces." << Log::Flush;
}

auto InterfaceManager::renderAsPythonDictionaryEntries() -> Str {
    //for(letcr p : modelMTMap) stringStream << "\"" << p.first << "\": " << p.second << ", ";

    StringStream ss;
    for(auto interface : interfaces) {
        auto parameters = interface->getParameters();
        for(const auto parameter : parameters)
            ss << "\"" << parameter->getCLName(true) << "\": " << parameter->valueToString() << ", ";
    }

    return ss.str();
}

auto InterfaceManager::renderParametersToString(StrVector params, Str separator) const -> Str {
    StringStream ss;

    for(auto interface : interfaces) {
        auto parameters = interface->getParameters();
        for(const auto parameter : parameters) {
            auto name = parameter->getCLName();

            if(Common::Contains(params, name))
                ss << name << "=" << parameter->valueToString() << separator;
        }
    }

    auto str = ss.str();

    return str.ends_with(separator) ? str.substr(0, str.length()-separator.length()) : str;
}

auto InterfaceManager::getInterface(const char *target) -> Interface::ConstPtr   {
    auto compFunc = [target](Interface::ConstPtr anInterface) { return anInterface->operator==(target); };

    auto it = std::find_if( interfaces.begin(), interfaces.end(), compFunc );

    if(it == interfaces.end())
        Log::WarningImportant() << "InterfaceManager could not find Interface " << Log::ForegroundCyan << target << Log::Flush;

    return *it;
}

//auto InterfaceManager::NewInterface(String name, InterfaceOwner *owner) -> Interface::Ptr {
//    auto newInterface = Interface::Ptr(new Interface(name, owner));
//
//    getInstance().registerInterface(newInterface);
//
//    return newInterface;
//}

