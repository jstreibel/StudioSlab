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
    CLOptionsDescription general("Get help");

    general.add_options()("help", "Help.");

    CLOptionsDescription allOptions;
    allOptions.add(general);

    auto interfaces = InterfaceManager::getInstance().getInterfaces();
    for(const auto interface : interfaces)
        allOptions.add(BuildOptionsDescription(*interface));

    CLVariablesMap vm;
    CLOptions::store(CLOptions::parse_command_line(argc, argv, allOptions), vm);

    InterfaceManager::getInstance().feedInterfaces(vm);

    if(vm.count("help")){
        allOptions.print(std::cout);
        exit(0);
    }
}

auto CLArgsManager::BuildOptionsDescription(const Interface &anInterface) -> CLOptionsDescription {
    auto desc = anInterface.getGeneralDescription();
    auto name = anInterface.getName() + (desc!=""? String(" (")+desc+")" : "") ;
    auto paramMap = anInterface.getParameters();

    CLOptionsDescription optDesc(name);
    CLODEasyInit options = optDesc.add_options();

    for(const auto p : paramMap){
        const String &paramName = p->getCommandLineArgName();

        options = p->getOptionDescription(options);
    }

    return optDesc;
}
