//
// Created by joao on 8/6/23.
//

#include "App.h"
#include "Base/Tools/Log.h"

MolecularDynamics::App::App(int argc, const char **argv) : AppBase(argc, argv) {
    Log::Success("Working!") << Log::Flush;
}

int MolecularDynamics::App::run() {

    return 0;
}

