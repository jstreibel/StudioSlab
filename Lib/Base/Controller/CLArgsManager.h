//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_COMMANDLINEINTERFACEMANAGER_H
#define FIELDS_COMMANDLINEINTERFACEMANAGER_H

#include "Base/Controller/Interface/Interface.h"
#include "Base/Controller/CLDefs.h"

class CLArgsManager {
    static CLArgsManager *singleton;

public:
    static auto GetInstance() -> CLArgsManager *;

    static void ShowHelp();
    static void Parse(int argc, const char **argv);

    static auto BuildOptionsDescription(const Interface &anInterface) -> CLOptionsDescription;

};


#endif //FIELDS_COMMANDLINEINTERFACEMANAGER_H
