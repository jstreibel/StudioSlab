//
// Created by joao on 8/6/23.
//

#include "App.h"

#include "Core/Tools/Log.h"
#include "Core/Controller/CLArgsManager.h"
#include "Math/Numerics/Program/Integrator.h"

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

