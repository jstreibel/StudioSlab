//
// Created by joao on 10/8/21.
//

#include "CrashPad.h"
#include "Core/Controller/CommandLine/CLInterfaceSelector.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include "LeadingDelta/LeadingDelta.h"

#include "MathApp.h"

int run(int argc, const char **argv) {
    auto input = Slab::New<Studios::Fields::R2toRLeadingDelta::Builder>( );

    Slab::Core::CLInterfaceManager::getInstance().registerInterface(input->getInterface());

    auto prog = Slab::Math::MathApp(argc, argv, input);

    return prog.run();
}

int main(int argc, const char **argv) {
    return Slab::SafetyNet::jump(run, argc, argv);
}
