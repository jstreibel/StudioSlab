//
// Created by joao on 10/13/21.
//

#include "CommandLineArgsManager.h"
#include "CommandLineParserDefs.h"
#include "../InterfaceManager.h"
#include "Core/Tools/Log.h"


namespace Slab::Core {

    void FCLArgsManager::ShowHelp() {
        throw "Show help not implemented";
    }

    void FCLArgsManager::Parse(int argc, const char **argv) {
        FLog::Info() << "FCLArgsManager parsing command line options." << FLog::Flush;

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

        FLog::Info() << "FCLArgsManager finished parsing command line options." << FLog::Flush;
    }

    auto FCLArgsManager::BuildOptionsDescription(const FInterface &Interface, CLOptionsDescription &Opts) -> void {
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
                FLog::Error() << "Couldn't add CLI option '" << Param->GetFullCommandLineName() << "' (" << Param->GetDescription()
                             << "): option already exists as '"
                             << same->GetFullCommandLineName() << "' (" << same->GetDescription() << ")." << FLog::Flush;
                throw e;
            }
    }


}