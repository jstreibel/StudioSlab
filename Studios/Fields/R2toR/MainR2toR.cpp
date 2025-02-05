//
// Created by joao on 10/8/21.
//

#include "CrashPad.h"
#include "Core/Controller/CommandLine/CLInterfaceSelector.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include "LeadingDelta/LeadingDelta.h"

#include "MathApp.h"

int run(int argc, const char **argv) {
    using namespace Slab;
    using namespace Slab::Core;


    CLInterfaceSelector selector("Simulation builder selector");
    auto option0 = Slab::New<Studios::Fields::R2toRLeadingDelta::Builder>( );

    /* sim 0 */selector.registerOption(option0->getInterface());
    /* sim 1 *///selector.registerOption(option1->getInterface());
    /* sim 2 *///selector.registerOption(option2->getInterface());
    /* sim 3 *///selector.registerOption(option3->getInterface());
    /* sim 4 *///selector.registerOption(option4->getInterface());

    auto selectedInterface = selector.preParse(argc, argv).getCurrentCandidate();
    auto input    = dynamic_cast<Models::KGR2toR::Builder*>(selectedInterface->getOwner());
    auto input_ptr = Pointer<Models::KGR2toR::Builder>(input);

    Slab::Core::CLInterfaceManager::getInstance().registerInterface(input->getInterface());

    auto prog = Math::MathApp(argc, argv, input_ptr);

    return prog.run();
}

int main(int argc, const char **argv) {
    return Slab::SafetyNet::jump(run, argc, argv);

    // return test();
}
