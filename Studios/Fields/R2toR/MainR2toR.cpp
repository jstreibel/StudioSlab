//
// Created by joao on 10/8/21.
//

#include "CrashPad.h"
#include "../../../Lib/Core/Controller/CommandLine/CommandLineInterfaceSelector.h"
#include "../../../Lib/Core/Controller/InterfaceManager.h"
#include "LeadingDelta/LeadingDelta.h"

#include "MathApp.h"

int run(int argc, const char **argv) {
    using namespace Slab;
    using namespace Slab::Core;


    CLInterfaceSelector selector("Simulation builder selector");
    auto option0 = Slab::New<Studios::Fields::R2toRLeadingDelta::Builder>( );

    /* sim 0 */selector.RegisterOption(option0->GetInterface());
    /* sim 1 *///selector.registerOption(option1->getInterface());
    /* sim 2 *///selector.registerOption(option2->getInterface());
    /* sim 3 *///selector.registerOption(option3->getInterface());
    /* sim 4 *///selector.registerOption(option4->getInterface());

    auto selectedInterface = selector.PreParse(argc, argv).GetCurrentCandidate();
    auto input    = dynamic_cast<Models::KGR2toR::Builder*>(selectedInterface->GetOwner());
    auto input_ptr = TPointer<Models::KGR2toR::Builder>(input);

    Slab::Core::FInterfaceManager::GetInstance().RegisterInterface(input->GetInterface());

    auto prog = Math::MathApp(argc, argv, input_ptr);

    return prog.run();
}

int main(int argc, const char **argv) {
    return Slab::SafetyNet::jump(run, argc, argv);

    // return test();
}
