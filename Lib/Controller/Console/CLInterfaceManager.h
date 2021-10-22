//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_COMMANDLINEINTERFACEMANAGER_H
#define FIELDS_COMMANDLINEINTERFACEMANAGER_H

#include "Controller/Interface/Interface.h"
#include "CLInterfaceDefs.h"

class CommandLineInterfaceManager {
    static CommandLineInterfaceManager *singleton;

public:
    static auto GetInstance() -> CommandLineInterfaceManager *;

    static void ShowHelp();
    static void Parse(int argc, const char **argv);

    static auto BuildOptionsDescription(const Interface &anInterface) -> CLOptionsDescription;

};


#endif //FIELDS_COMMANDLINEINTERFACEMANAGER_H
