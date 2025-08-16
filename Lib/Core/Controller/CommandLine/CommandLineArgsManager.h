//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_COMMANDLINEINTERFACEMANAGER_H
#define FIELDS_COMMANDLINEINTERFACEMANAGER_H

#include "../Interface.h"
#include "CommandLineParserDefs.h"


namespace Slab::Core {

    class CLArgsManager {
        CLArgsManager() = delete;

    public:
        static void ShowHelp();

        static void Parse(int argc, const char **argv);

        static auto BuildOptionsDescription(const FInterface &anInterface, CLOptionsDescription &opts) -> void;

    };


}

#endif //FIELDS_COMMANDLINEINTERFACEMANAGER_H
