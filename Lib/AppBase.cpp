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

        Log::Info() << "Compiler: " << USED_CXX_COMPILER << Log::Flush;
        // Log::Info() << "Compiler: " << COMPILER_NAME << Log::Flush;
        Log::Info() << "PWD: " << Common::GetPWD() << Log::Flush;
    }

    AppBase::~AppBase() {
        Core::Finish();
        Log::Info() << Common::getClassName(this) << " terminated." << Log::Flush;
    }

}