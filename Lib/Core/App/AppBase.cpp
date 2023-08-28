//
// Created by joao on 11/4/21.
//

#include "AppBase.h"

#include <Core/Controller/CLArgsManager.h>

#include "Core/Controller/Interface/InterfaceManager.h"
#include "Core/Tools/Log.h"


AppBase::AppBase(int argc, const char **argv, bool doRegister)
    : InterfaceOwner("App", 100, doRegister) {

    CLArgsManager::Initialize(argc, argv);
}

AppBase::~AppBase() {
    Log::Info() << Common::getClassName(this) << " destroyed" << Log::Flush;
}

void AppBase::parseCLArgs() {
    CLArgsManager::Parse();
}



