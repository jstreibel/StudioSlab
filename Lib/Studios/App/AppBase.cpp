//
// Created by joao on 11/4/21.
//

#include "AppBase.h"

#include <Studios/Controller/CLArgsManager.h>

AppBase::AppBase(int argc, const char **argv)
    : Interface("App"), argc(argc), argv(argv) {
}

void AppBase::parseCLArgs() {
    CLArgsManager::Parse(argc, argv);
}


