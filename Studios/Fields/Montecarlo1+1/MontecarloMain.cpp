//
// Created by joao on 3/12/23.
//

#include <Studios/App/CrashPad.h>
#include <Studios/Controller/Interface/InterfaceSelector.h>

#include <Fields/Mappings/RtoR/App/FieldsApp-RtoR.h>

#include <Fields/Mappings/RtoR/Controller/ManyOscillons/InputManyOscillons.h>
#include <Fields/Mappings/RtoR/Controller/ManyOscillons/InputMachineGun.h>
#include <Fields/Mappings/RtoR/Controller/ManyOscillons/InputStatistical.h>
#include <Fields/Mappings/RtoR/Controller/ManyOscillons/InputRandomEnergyOverDotPhi.h>


int main(int argc, const char **argv) {

    auto &selector = InterfaceSelector::getInstance();

    /* sim 0 */selector.registerOption(new RtoR::InputStatistical);
    /* sim 1 */selector.registerOption(new RtoR::InputMachineGun);
    /* sim 2 */selector.registerOption(new RtoR::InputManyOscillons);
    /* sim 3 */selector.registerOption(new RtoR::InputRandomEnergyOverDotPhi);

    selector.setup(argc, argv);

    auto prog = SimulationsAppRtoR(argc, argv, SimulationsAppRtoR::Integration::montecarlo);

    return SafetyNet::jump(prog);
}