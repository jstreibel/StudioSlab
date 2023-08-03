//
// Created by joao on 27/09/2021.
//

#include "Base/App/CrashPad.h"
#include "Base/Controller/Interface/InterfaceSelector.h"

#include "Phys/App.h"

#include "ThermalStudies/InputManyOscillons.h"
#include "ThermalStudies/InputMachineGun.h"
#include "ThermalStudies/InputStatistical.h"
#include "ThermalStudies/InputRandomEnergyOverDotPhi.h"

int run(int argc, const char **argv){
    InterfaceSelector selector("Dynamic thermal");
    std::vector<RtoR::Builder*>
            options = { new RtoR::InputStatistical,
                        new RtoR::InputMachineGun,
                        new RtoR::InputManyOscillons,
                        new RtoR::InputRandomEnergyOverDotPhi };

    for(auto &opt : options)
        selector.registerOption(opt->getInterface());

    auto selection = dynamic_cast<RtoR::Builder*>(
            selector.preParse(argc, argv).getCurrentCandidate()->getOwner());

    auto prog = Simulation::App(argc, argv, RtoR::Builder::Ptr(selection));

    return prog.run();
}

int main(int argc, const char **argv) {
    return SafetyNet::jump(run, argc, argv);
}


