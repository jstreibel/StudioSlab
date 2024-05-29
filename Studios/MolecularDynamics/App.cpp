//
// Created by joao on 8/6/23.
//

#include "App.h"

#include "Core/Tools/Log.h"
#include "Core/Controller/CommandLine/CLArgsManager.h"
#include "Math/Numerics/Program/NumericTask.h"
#include "Core/Backend/BackendManager.h"

#define SFML_Backend dynamic_cast<SFMLBackend&>(Core::BackendManager::GetGUIBackend())

MolecularDynamics::App::App(int argc, const char **argv)
: AppBase(argc, argv), backend(SFML_Backend) {
    CLArgsManager::Parse();
}

int MolecularDynamics::App::run() {
    NumericalIntegration integrator(builder);

    backend.run(&integrator);

    Log::Info() << "MolecularDynamics finished." << Log::Flush;

    return true;
}

