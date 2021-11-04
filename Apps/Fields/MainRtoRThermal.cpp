//
// Created by joao on 27/09/2021.
//

#include <Studios/App/CrashPad.h>
#include <Studios/Controller/Interface/InterfaceSelector.h>

#include <Fields/Mappings/RtoR/App/FieldsApp-RtoR.h>

#include <Fields/Mappings/RtoR/Controller/ManyOscillons/InputManyOscillons.h>
#include <Fields/Mappings/RtoR/Controller/ManyOscillons/InputMachineGun.h>
#include <Fields/Mappings/RtoR/Controller/ManyOscillons/InputStatistical.h>


int main(int argc, const char **argv) {

    auto &im = InterfaceSelector::getInstance();

    /* sim 0 */im.registerCandidate(new RtoR::InputStatistical);
    /* sim 1 */im.registerCandidate(new RtoR::InputMachineGun);
    /* sim 2 */im.registerCandidate(new RtoR::InputManyOscillons);

    im.setup(argc, argv);

    auto prog = SimulationsAppRtoR(argc, argv);

    return SafetyNet::jump(prog);
}


