//
// Created by joao on 27/09/2021.
//

#include "Base/App/CrashPad.h"
#include "Base/Controller/Interface/InterfaceSelector.h"

#include "Phys/App.h"

#include "ThermalStudies/__archive__/ManyOscillonsBuilder.h"
#include "ThermalStudies/__archive__/MachineGunBuilder.h"
#include "ThermalStudies/__archive__/StatisticalBuilder.h"
#include "ThermalStudies/__archive__/RandomEnergyOverDotPhiBuilder.h"

int run(int argc, const char **argv){
    InterfaceSelector selector("Dynamic thermal");
    std::vector<RtoR::Builder*>
            options = { new RtoR::StatisticalBuilder,
                        new RtoR::MachineGunBuilder,
                        new RtoR::ManyOscillonsBuilder,
                        new RtoR::RandomEnergyOverDotPhiBuilder };

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


