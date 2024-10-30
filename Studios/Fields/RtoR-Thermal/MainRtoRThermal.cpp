//
// Created by joao on 27/09/2021.
//

#include "CrashPad.h"
#include "Core/Controller/CommandLine/CLInterfaceSelector.h"

#include "MathApp.h"

#include "Studies/ManyOscillonsBuilder.h"
#include "Studies/MachineGunBuilder.h"
#include "Studies/StatisticalBuilder.h"
#include "Studies/RandomEnergyOverDotPhiBuilder.h"

int run(int argc, const char **argv){
    using namespace Slab::Core;
    using namespace Slab::Math;
    using namespace Studios::Fields::RtoRThermal;

    CLInterfaceSelector selector("Dynamic thermal");
    Vector<Slab::Models::KGRtoR::KGRtoRBuilder*>
            options = { new StatisticalBuilder,
                        new MachineGunBuilder,
                        new ManyOscillonsBuilder,
                        new RandomEnergyOverDotPhiBuilder };

    for(auto &opt : options)
        selector.registerOption(opt->getInterface());

    auto selection = dynamic_cast<Slab::Models::KGRtoR::KGRtoRBuilder*>(
            selector.preParse(argc, argv).getCurrentCandidate()->getOwner());

    auto prog = MathApp(argc, argv, Slab::Models::KGRtoR::KGRtoRBuilder_ptr(selection));

    return prog.run();
}

int main(int argc, const char **argv) {
    return Slab::Core::SafetyNet::jump(run, argc, argv);
}


