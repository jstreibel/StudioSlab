//
// Created by joao on 8/6/23.
//

#include "App.h"

#include "Base/Tools/Log.h"
#include "Base/Controller/CLArgsManager.h"
#include "Phys/Numerics/Program/Integrator.h"

MolecularDynamics::App::App(int argc, const char **argv)
: AppBase(argc, argv), backend(Backend::Initialize<SFMLNuklearBackend>()) {
    CLArgsManager::Parse();
}

int MolecularDynamics::App::run() {
    NumericalIntegration integrator(builder);

    backend.run(&integrator);

    Log::Info() << "MolecularDynamics finished." << Log::Flush;

    return true;
}

