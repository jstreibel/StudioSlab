//
// Created by joao on 27/09/2021.
//

#include "Lib/App/SafetyNet.h"
#include "Apps/Maps/RtoR/App/SimulationsAppRtoR.h"

#include <Apps/Maps/RtoR/Controller/ManyOscillons/InputManyOscillons.h>
#include <Apps/Maps/RtoR/Controller/ManyOscillons/InputMachineGun.h>
#include <Apps/Maps/RtoR/Controller/ManyOscillons/InputStatistical.h>


int main(int argc, const char **argv) {

    auto &im = CLInterfaceSelector::getInstance();

    /* sim 0 */im.registerInterface(new RtoR::InputStatistical);
    /* sim 1 */im.registerInterface(new RtoR::InputMachineGun);
    /* sim 2 */im.registerInterface(new RtoR::InputManyOscillons);

    auto prog = SimulationsAppRtoR(argc, argv);

    return SafetyNet::jump(argc, argv, prog);
}


