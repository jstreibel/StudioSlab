//
// Created by joao on 10/13/21.
//

#include "CLArgsManager.h"
#include "CLDefs.h"
#include "CLInterfaceManager.h"
#include "Core/Tools/Log.h"


namespace Slab::Core {

    CLArgsManager *CLArgsManager::singleton = nullptr;

    CLArgsManager::CLArgsManager(int argc, const char **argv)
            : argc(argc), argv(argv) {

        auto &log = Log::Status() << "CLArgsManager initialized with " << argc << " argument" << (argc > 1 ? "s" : "")
                                  << ": " << Log::Flush;
        for (int i = 0; i < argc; ++i)
            log << "[" << argv[i] << "] ";

        log << Log::Flush;
    }

    auto CLArgsManager::Initialize(int argc, const char **argv) -> CLArgsManager * {
        if (singleton != nullptr) throw Str("CLArgsManager has already been initialized!");

        CLArgsManager::singleton = new CLArgsManager(argc, argv);
        return CLArgsManager::GetInstance();
    }

    auto CLArgsManager::GetInstance() -> CLArgsManager * {
        if (singleton == nullptr) throw Str("CLArgsManager has not been initialized!");

        return singleton;
    }

    void CLArgsManager::ShowHelp() {
        throw "Show help not implemented";
    }

    void CLArgsManager::Parse() {
        Log::Critical() << "CLArgsManager started parsing command line options." << Log::Flush;

        CLOptionsDescription allOptions("Pendulum");

        allOptions.add_options("General")("help", "Print this help.");

        auto interfaces = CLInterfaceManager::getInstance().getInterfaces();
        for (const auto interface: interfaces) {
            BuildOptionsDescription(*interface, allOptions);
            allOptions.add_options();
        }

        fix argc = CLArgsManager::GetInstance()->argc;
        fix argv = CLArgsManager::GetInstance()->argv;

        let result = allOptions.parse(argc, argv);

        CLInterfaceManager::getInstance().feedInterfaces(result);

        if (result.count("help")) {
            std::cout << allOptions.help();
            exit(0);
        }

        Log::Success() << "CLArgsManager finished parsing command line options." << Log::Flush;
    }

    auto CLArgsManager::BuildOptionsDescription(const CLInterface &anInterface, CLOptionsDescription &opts) -> void {
        auto desc = anInterface.getGeneralDescription();
        auto name = anInterface.getName() + (desc != "" ? Str(" (") + desc + ")" : "");

        auto group = opts.add_options(name);

        auto paramMap = anInterface.getParameters();

        for (const auto p: paramMap)
            try {
                p->addToCommandLineOptionsGroup(group);
            }
            catch (cxxopts::exceptions::option_already_exists &e) {
                fix same = CLInterfaceManager::getInstance().getParameter(p->getCommandLineArgumentName());
                Log::Error() << "Couldn't add CLI option '" << p->getFullCommandLineName() << "' (" << p->getDescription()
                             << "): option already exists as '"
                             << same->getFullCommandLineName() << "' (" << same->getDescription() << ")." << Log::Flush;
                throw e;
            }
    }


}