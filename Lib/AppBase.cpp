//
// Created by joao on 11/4/21.
//

#include "AppBase.h"

#include "Core/Controller/CommandLine/CLArgsManager.h"

#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include "Core/Tools/Log.h"
#include "Utils/Compiler.h"

#include "StudioSlab.h"

namespace Slab::Core {

    AppBase::AppBase(int argc, const char **argv, bool doRegister)
            : CLInterfaceOwner("App", 100, doRegister) {
        Slab::Startup();

        CLArgsManager::Initialize(argc, argv);

        Log::Info() << "Compiler: " << USED_CXX_COMPILER << Log::Flush;
        // Log::Info() << "Compiler: " << COMPILER_NAME << Log::Flush;
        Log::Info() << "PWD: " << Common::GetPWD() << Log::Flush;
    }

    AppBase::~AppBase() {
        Log::Info() << Common::getClassName(this) << " terminated." << Log::Flush;
    }

    void AppBase::parseCLArgs() {
        CLArgsManager::Parse();
    }


}