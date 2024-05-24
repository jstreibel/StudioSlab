//
// Created by joao on 27/09/2021.
//

#include "Core/App/CrashPad.h"
#include "Core/Controller/Interface/InterfaceSelector.h"

#include "Math/App.h"

#include "Studies/ManyOscillonsBuilder.h"
#include "Studies/MachineGunBuilder.h"
#include "Studies/StatisticalBuilder.h"
#include "Studies/RandomEnergyOverDotPhiBuilder.h"

int run(int argc, const char **argv){
    using namespace Slab::Core;
    using namespace Slab::Math;
    using namespace Studios::Fields::RtoRThermal;

    InterfaceSelector selector("Dynamic thermal");
    std::vector<RtoR::KGBuilder*>
            options = { new StatisticalBuilder,
                        new MachineGunBuilder,
                        new ManyOscillonsBuilder,
                        new RandomEnergyOverDotPhiBuilder };

    for(auto &opt : options)
        selector.registerOption(opt->getInterface());

    auto selection = dynamic_cast<RtoR::KGBuilder*>(
            selector.preParse(argc, argv).getCurrentCandidate()->getOwner());

    auto prog = App(argc, argv, RtoR::KGBuilder::Ptr(selection));

    return prog.run();
}

int main(int argc, const char **argv) {
    return Slab::Core::SafetyNet::jump(run, argc, argv);
}


