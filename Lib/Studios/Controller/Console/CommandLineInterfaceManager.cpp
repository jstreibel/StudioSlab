//
// Created by joao on 10/13/21.
//

#include "CLInterfaceManager.h"
#include "CLInterfaceDefs.h"
#include "Studios/Controller/Interface/InterfaceManager.h"
#include "CLInterfaceSelector.h"

CommandLineInterfaceManager* CommandLineInterfaceManager::singleton = nullptr;


auto CommandLineInterfaceManager::GetInstance() -> CommandLineInterfaceManager * {
    if(singleton == nullptr)
        singleton = new CommandLineInterfaceManager;

    return singleton;
}

void CommandLineInterfaceManager::ShowHelp() {

}

void CommandLineInterfaceManager::Parse(int argc, const char **argv) {
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
        exit(1);
    }
}

auto CommandLineInterfaceManager::BuildOptionsDescription(const Interface &anInterface) -> CLOptionsDescription {
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
