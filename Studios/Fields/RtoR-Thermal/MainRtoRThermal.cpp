//
// Created by joao on 27/09/2021.
//

#include "Base/App/CrashPad.h"
#include "Base/Controller/Interface/InterfaceSelector.h"

#include "Phys/App.h"

#include "Studies/ManyOscillonsBuilder.h"
#include "Studies/MachineGunBuilder.h"
#include "Studies/StatisticalBuilder.h"
#include "Studies/RandomEnergyOverDotPhiBuilder.h"

int run(int argc, const char **argv){
    InterfaceSelector selector("Dynamic thermal");
    std::vector<RtoR::KGBuilder*>
            options = { new RtoR::StatisticalBuilder,
                        new RtoR::MachineGunBuilder,
                        new RtoR::ManyOscillonsBuilder,
                        new RtoR::RandomEnergyOverDotPhiBuilder };

    for(auto &opt : options)
        selector.registerOption(opt->getInterface());

    auto selection = dynamic_cast<RtoR::KGBuilder*>(
            selector.preParse(argc, argv).getCurrentCandidate()->getOwner());

    auto prog = Simulation::App(argc, argv, RtoR::KGBuilder::Ptr(selection));

    return prog.run();
}

int main(int argc, const char **argv) {
    return SafetyNet::jump(run, argc, argv);
}


