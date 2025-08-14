//
// Created by joao on 10/13/21.
//

#include "CommandLineArgsManager.h"
#include "CommandLineParserDefs.h"
#include "CommandLineInterfaceManager.h"
#include "Core/Tools/Log.h"


namespace Slab::Core {

    void CLArgsManager::ShowHelp() {
        throw "Show help not implemented";
    }

    void CLArgsManager::Parse(int argc, const char **argv) {
        Log::Info() << "CLArgsManager parsing command line options." << Log::Flush;

        CLOptionsDescription allOptions("Pendulum");

        allOptions.add_options("General")("help", "Print this help.");

        auto interfaces = FCommandLineInterfaceManager::getInstance().getInterfaces();
        for (const auto interface: interfaces) {
            BuildOptionsDescription(*interface, allOptions);
            allOptions.add_options();
        }

        let result = allOptions.parse(argc, argv);

        FCommandLineInterfaceManager::getInstance().feedInterfaces(result);

        if (result.count("help")) {
            std::cout << allOptions.help();
            exit(0);
        }

        Log::Info() << "CLArgsManager finished parsing command line options." << Log::Flush;
    }

    auto CLArgsManager::BuildOptionsDescription(const FCommandLineInterface &anInterface, CLOptionsDescription &opts) -> void {
        auto desc = anInterface.GetGeneralDescription();
        auto name = anInterface.GetName() + (desc != "" ? Str(" (") + desc + ")" : "");

        auto group = opts.add_options(name);

        auto paramMap = anInterface.GetParameters();

        for (const auto p: paramMap)
            try {
                p->AddToCommandLineOptionsGroup(group);
            }
            catch (cxxopts::exceptions::option_already_exists &e) {
                fix same = FCommandLineInterfaceManager::getInstance().getParameter(p->getCommandLineArgumentName());
                Log::Error() << "Couldn't add CLI option '" << p->getFullCommandLineName() << "' (" << p->getDescription()
                             << "): option already exists as '"
                             << same->getFullCommandLineName() << "' (" << same->getDescription() << ")." << Log::Flush;
                throw e;
            }
    }


}