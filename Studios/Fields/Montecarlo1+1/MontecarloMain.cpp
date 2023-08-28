//
// Created by joao on 3/12/23.
//

#include <Core/App/CrashPad.h>
#include <Core/Controller/Interface/InterfaceManager.h>

#include "Mappings/RtoR/Controller/ThermalStudies/InputRandomEnergyOverDotPhi.h"

#include "Montecarlo.h"

int main(int argc, const char **argv) {

    auto montecarlo = Montecarlo::Input::Ptr(new Montecarlo::Input);
    InterfaceManager::getInstance().registerInterface(montecarlo->getInterface());

    SimulationsAppRtoR prog(argc, argv,  montecarlo);

    return SafetyNet::jump(prog);
}