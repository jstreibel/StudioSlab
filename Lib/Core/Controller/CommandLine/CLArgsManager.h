//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_COMMANDLINEINTERFACEMANAGER_H
#define FIELDS_COMMANDLINEINTERFACEMANAGER_H

#include "CLInterface.h"
#include "CLDefs.h"


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

        static auto BuildOptionsDescription(const CLInterface &anInterface, CLOptionsDescription &opts) -> void;

    };


}

#endif //FIELDS_COMMANDLINEINTERFACEMANAGER_H
