//
// Created by joao on 11/4/21.
//

#include "AppBase.h"

#include <Base/Controller/CLArgsManager.h>

AppBase::AppBase(int argc, const char **argv)
    : Interface("App"), argc(argc), argv(argv) {

    for(auto i=0; i<argc; ++i) args.emplace_back(String(argv[i]));
}

void AppBase::parseCLArgs() {
    CLArgsManager::Parse(argc, argv);
}


