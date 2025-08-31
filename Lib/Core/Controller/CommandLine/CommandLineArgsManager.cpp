//
// Created by joao on 10/13/21.
//

#include "CommandLineArgsManager.h"
#include "CommandLineParserDefs.h"
#include "../InterfaceManager.h"
#include "Core/Tools/Log.h"


namespace Slab::Core {

    void CLArgsManager::ShowHelp() {
        throw "Show help not implemented";
    }

    void CLArgsManager::Parse(int argc, const char **argv) {
        Log::Info() << "CLArgsManager parsing command line options." << Log::Flush;

        CLOptionsDescription allOptions("StudioSlab");

        allOptions.add_options("General")("help", "Print this help.");

        auto interfaces = FInterfaceManager::GetInstance().GetInterfaces();
        for (const auto interface: interfaces) {
            BuildOptionsDescription(*interface, allOptions);
            allOptions.add_options();
        }

        let result = allOptions.parse(argc, argv);

        FInterfaceManager::GetInstance().FeedInterfaces(result);

        if (result.count("help")) {
            std::cout << allOptions.help();
            exit(0);
        }

        Log::Info() << "CLArgsManager finished parsing command line options." << Log::Flush;
    }

    auto CLArgsManager::BuildOptionsDescription(const FInterface &Interface, CLOptionsDescription &Opts) -> void {
        fix Desc = Interface.GetGeneralDescription();
        fix Name = Interface.GetName() + (Desc != "" ? Str(" (") + Desc + ")" : "");

        auto Group = Opts.add_options(Name);

        auto ParamMap = Interface.GetParameters();

        for (const auto Param: ParamMap)
            try {
                Param->AddToCommandLineOptionsGroup(Group);
            }
            catch (cxxopts::exceptions::option_already_exists &e) {
                fix same = FInterfaceManager::GetInstance().GetParameter(Param->GetCommandLineArgumentName());
                Log::Error() << "Couldn't add CLI option '" << Param->GetFullCommandLineName() << "' (" << Param->GetDescription()
                             << "): option already exists as '"
                             << same->GetFullCommandLineName() << "' (" << same->GetDescription() << ")." << Log::Flush;
                throw e;
            }
    }


}