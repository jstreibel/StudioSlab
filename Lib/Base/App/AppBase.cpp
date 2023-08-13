//
// Created by joao on 11/4/21.
//

#include "AppBase.h"

#include <Base/Controller/CLArgsManager.h>

#include "Base/Controller/Interface/InterfaceManager.h"
#include "Base/Tools/Log.h"


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



