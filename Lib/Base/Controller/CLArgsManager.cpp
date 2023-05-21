//
// Created by joao on 10/13/21.
//

#include "CLArgsManager.h"
#include "Base/Controller/CLDefs.h"
#include "Base/Controller/Interface/InterfaceManager.h"

CLArgsManager* CLArgsManager::singleton = nullptr;


auto CLArgsManager::GetInstance() -> CLArgsManager * {
    if(singleton == nullptr)
        singleton = new CLArgsManager;

    return singleton;
}

void CLArgsManager::ShowHelp() {
    std::cout << "Show help not implemented";
    throw        "Show help not implemented";
}

void CLArgsManager::Parse(int argc, const char **argv) {
    po::options_description general("Get help");

    general.add_options()("help,h", "Help.");

    po::options_description allOptions;
    allOptions.add(general);

    auto interfaces = InterfaceManager::getInstance().getInterfaces();
    for(const auto *interface : interfaces)
        allOptions.add(BuildOptionsDescription(*interface));

    CLVariablesMap vm;
    boost::program_options::store(po::parse_command_line(argc, argv, allOptions), vm);

    InterfaceManager::getInstance().feedInterfaces(vm);

    if(vm.count("help")){
        allOptions.print(std::cout);
        exit(0);
    }
}

auto CLArgsManager::BuildOptionsDescription(const Interface &anInterface) -> CLOptionsDescription {
    auto description = anInterface.getGeneralDescription();
    auto paramMap = anInterface.getParameters();

    po::options_description desc(description);
    po::options_description_easy_init options = desc.add_options();

    for(const auto* p : paramMap){
        const String &paramName = p->getCommandLineArgName();

        options = p->getOptionDescription(options);
    }

    return desc;
}
