//
// Created by joao on 27/09/2021.
//

#include <Studios/App/CrashPad.h>
#include <Studios/Controller/Interface/InterfaceSelector.h>

#include <Fields/Mappings/RtoR/App/FieldsApp-RtoR.h>

#include <Fields/Mappings/RtoR/Controller/ThermalStudies/InputManyOscillons.h>
#include <Fields/Mappings/RtoR/Controller/ThermalStudies/InputMachineGun.h>
#include <Fields/Mappings/RtoR/Controller/ThermalStudies/InputStatistical.h>
#include <Fields/Mappings/RtoR/Controller/ThermalStudies/InputRandomEnergyOverDotPhi.h>


int main(int argc, const char **argv) {

    auto &selector = InterfaceSelector::getInstance();

    /* sim 0 */selector.registerOption(new RtoR::InputStatistical);
    /* sim 1 */selector.registerOption(new RtoR::InputMachineGun);
    /* sim 2 */selector.registerOption(new RtoR::InputManyOscillons);
    /* sim 3 */selector.registerOption(new RtoR::InputRandomEnergyOverDotPhi);

    selector.setup(argc, argv);

    auto prog = SimulationsAppRtoR(argc, argv);

    return SafetyNet::jump(prog);
}


