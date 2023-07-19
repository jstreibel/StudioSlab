//
// Created by joao on 27/09/2021.
//

#include "Base/App/CrashPad.h"
#include "Base/Controller/Interface/InterfaceSelector.h"

#include "Mappings/RtoR/App/FieldsApp-RtoR.h"

#include "Mappings/RtoR/Controller/ThermalStudies/InputManyOscillons.h"
#include "Mappings/RtoR/Controller/ThermalStudies/InputMachineGun.h"
#include "Mappings/RtoR/Controller/ThermalStudies/InputStatistical.h"
#include "Mappings/RtoR/Controller/ThermalStudies/InputRandomEnergyOverDotPhi.h"


int main(int argc, const char **argv) {

    InterfaceSelector selector("Dynamic thermal");
    std::vector<RtoRBCInterface*>
    options = { new RtoR::InputStatistical,
                new RtoR::InputMachineGun,
                new RtoR::InputManyOscillons,
                new RtoR::InputRandomEnergyOverDotPhi };

    for(auto &opt : options)
        selector.registerOption(opt->getInterface());

    auto selection = dynamic_cast<RtoRBCInterface*>(
            selector.preParse(argc, argv).getCurrentCandidate()->getOwner());

    auto prog = SimulationsAppRtoR(argc, argv, RtoRBCInterface::Ptr(selection));

    return SafetyNet::jump(prog);
}


