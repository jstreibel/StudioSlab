//
// Created by joao on 8/6/23.
//

#include "App.h"
#include "Base/Tools/Log.h"
#include "Base/Backend/SFML-Nuklear/SFML-Nuklear-Backend.h"
#include "Base/Controller/CLArgsManager.h"


MolecularDynamics::App::App(int argc, const char **argv) : AppBase(argc, argv) {
    Backend::Initialize<SFMLNuklearBackend>();

    CLArgsManager::GetInstance()->Parse(argc, argv);
}

int MolecularDynamics::App::run() {

    return 0;
}

