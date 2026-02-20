//
// Created by joao on 11/4/21.
//

#include "AppBase.h"

#include "Core/Controller/CommandLine/CommandLineArgsManager.h"

#include "Core/Controller/InterfaceManager.h"
#include "Core/Tools/Log.h"
#include "Utils/Compiler.h"

#include "StudioSlab.h"
#include "Core/SlabCore.h"

namespace Slab::Core {

    AppBase::AppBase(int argc, const char **argv, bool doRegister)
            : FInterfaceOwner("App", 100, doRegister) {
        Slab::Startup();

        FLog::Info() << "Compiler: " << USED_CXX_COMPILER << FLog::Flush;
        // FLog::Info() << "Compiler: " << COMPILER_NAME << FLog::Flush;
        FLog::Info() << "PWD: " << Common::GetPWD() << FLog::Flush;
    }

    AppBase::~AppBase() {
        Core::Finish();
        FLog::Info() << Common::GetClassName(this) << " terminated." << FLog::Flush;
    }

}