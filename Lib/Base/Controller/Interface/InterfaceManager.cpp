//
// Created by joao on 10/17/21.
//

#include <Common/Workaround/StringStream.h>
#include "InterfaceManager.h"

InterfaceManager *InterfaceManager::instance = nullptr;

auto InterfaceManager::getInstance() -> InterfaceManager & {
    if(instance == nullptr) instance = new InterfaceManager;

    return *instance;
}

void InterfaceManager::registerInterface(Interface *anInterface) {
    //std::cout << "\nRegistering interface \"" << anInterface->getGeneralDescription() << "\"... ";
    interfaces.emplace_back(anInterface);

    //auto size = anInterface->parameters.size();
    //std::cout << "Registered with " << size << " parameters" << (size?":":".");

    //for(auto *p : anInterface->parameters) {
    //    std::cout << "\n\t" << p->getCommandLineArgName() << ": " << std::flush;
    //    std::cout << p->valueToString() << std::flush;
    //}

    //std::cout << std::endl;
}

auto InterfaceManager::getInterfaces() -> std::vector<const Interface *> {
    std::vector<const Interface*> V(interfaces.size());

    std::copy(interfaces.begin(), interfaces.end(), V.begin());

    return V;
}

void InterfaceManager::feedInterfaces(CLVariablesMap vm) {
    for(auto *interface : interfaces){
        // TODO passar (somehow) para as interfaces somente as variaveis que importam, e não todas o tempo todo.
        // Ocorre que, passando todas sempre, certas interfaces terao acesso a informacao que nao lhes interessa.

        interface->setup(vm);
    }
}

auto InterfaceManager::renderAsPythonDictionaryEnrties() -> String {
    //for(letcr p : modelMTMap) stringStream << "\"" << p.first << "\": " << p.second << ", ";

    StringStream ss;
    for(auto *interface : interfaces) {
        auto parameters = interface->getParameters();
        for(const auto *parameter : parameters)
            ss << "\"" << parameter->getCommandLineArgName(true) << "\": " << parameter->valueToString() << ", ";
    }

    return ss.str();
}