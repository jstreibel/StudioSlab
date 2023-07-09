//
// Created by joao on 10/13/21.
//

#include "CLArgsManager.h"
#include "CLDefs.h"
#include "Interface/InterfaceManager.h"

CLArgsManager* CLArgsManager::singleton = nullptr;


auto CLArgsManager::GetInstance() -> CLArgsManager * {
    if(singleton == nullptr)
        singleton = new CLArgsManager;

    return singleton;
}

void CLArgsManager::ShowHelp() {
    throw "Show help not implemented";
}

void CLArgsManager::Parse(int argc, const char **argv) {
    CLOptionsDescription allOptions("Pendulum");

    allOptions.add_options("General")("help", "Print this help.");
    // allOptions.add_options(general);

    auto interfaces = InterfaceManager::getInstance().getInterfaces();
    for(const auto interface : interfaces) {
        BuildOptionsDescription(*interface, allOptions);
        allOptions.add_options();
    }

    auto result = allOptions.parse(argc, argv);

    InterfaceManager::getInstance().feedInterfaces(result);

    if(result.count("help")){
        std::cout << allOptions.help();
        exit(0);
    }
}

auto CLArgsManager::BuildOptionsDescription(const Interface &anInterface, CLOptionsDescription &opts) -> void {
    auto desc = anInterface.getGeneralDescription();
    auto name = anInterface.getName() + (desc!="" ? Str(" (") + desc + ")" : "") ;

    auto group = opts.add_options(name);

    auto paramMap = anInterface.getParameters();
    for(const auto p : paramMap)
        p->addToOptionsGroup(group);
}
