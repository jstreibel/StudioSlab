//
// Created by joao on 3/12/23.
//

#include <Studios/App/CrashPad.h>
#include <Studios/Controller/Interface/InterfaceSelector.h>
#include <Fields/Mappings/RtoR/App/FieldsApp-RtoR.h>

#include <Fields/Mappings/RtoR/Controller/ThermalStudies/InputRandomEnergyOverDotPhi.h>

#include "Montecarlo.h"





int main(int argc, const char **argv) {

    auto &selector = InterfaceSelector::getInstance();
    /* Sim 0 */ selector.registerOption(new Montecarlo::Input);

    selector.setup(argc, argv);

    auto prog = SimulationsAppRtoR(argc, argv, SimulationsAppRtoR::Integration::montecarlo);

    return SafetyNet::jump(prog);
}