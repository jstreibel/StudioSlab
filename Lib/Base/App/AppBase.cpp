//
// Created by joao on 11/4/21.
//

#include "AppBase.h"

#include <Base/Controller/CLArgsManager.h>

#include "Base/Controller/Interface/InterfaceManager.h"


AppBase::AppBase(int argc, const char **argv)
    : InterfaceOwner("App", 100, true), argc(argc), argv(argv) {

    for(auto i=0; i<argc; ++i) args.emplace_back(Str(argv[i]));

}

void AppBase::parseCLArgs() {
    CLArgsManager::Parse(argc, argv);
}


