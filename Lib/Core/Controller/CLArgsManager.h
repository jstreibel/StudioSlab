//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_COMMANDLINEINTERFACEMANAGER_H
#define FIELDS_COMMANDLINEINTERFACEMANAGER_H

#include "Core/Controller/Interface/Interface.h"
#include "Core/Controller/CLDefs.h"


namespace Slab::Core {

    class CLArgsManager {
        static CLArgsManager *singleton;

        int argc;
        const char **argv;

        CLArgsManager() = delete;

        CLArgsManager(int argc, const char **argv);

    public:
        static auto Initialize(int argc, const char **argv) -> CLArgsManager *;

        static auto GetInstance() -> CLArgsManager *;

        static void ShowHelp();

        static void Parse();

        static auto BuildOptionsDescription(const Interface &anInterface, CLOptionsDescription &opts) -> void;

    };


}

#endif //FIELDS_COMMANDLINEINTERFACEMANAGER_H
