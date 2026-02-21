//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_COMMANDLINEINTERFACEMANAGER_H
#define FIELDS_COMMANDLINEINTERFACEMANAGER_H

#include "../Interface.h"
#include "CommandLineParserDefs.h"


namespace Slab::Core {

    class FCLArgsManager {
        FCLArgsManager() = delete;

    public:
        static void ShowHelp();

        static void Parse(int argc, const char **argv);

        static auto BuildOptionsDescription(const FInterface &Interface, CLOptionsDescription &Opts) -> void;

    };

    using CLArgsManager [[deprecated("Use FCLArgsManager")]] = FCLArgsManager;

}

#endif //FIELDS_COMMANDLINEINTERFACEMANAGER_H
