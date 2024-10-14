//
// Created by joao on 3/12/23.
//

#include <Core/App/CrashPad.h>
#include "Core/Controller/CommandLine/CLInterfaceManager.h"

#include "Math/Function/RtoR/Controller/ThermalStudies/InputRandomEnergyOverDotPhi.h"

#include "Montecarlo.h"

int main(int argc, const char **argv) {

    auto montecarlo = Slab::New<Montecarlo::Input>();
    // Slab::Core::InterfaceManager::getInstance().registerInterface(montecarlo->getInterface());

    SimulationsAppRtoR prog(argc, argv,  montecarlo);

    return SafetyNet::jump(prog);
}