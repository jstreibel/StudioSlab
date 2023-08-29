//
// Created by joao on 10/8/21.
//

#include "Core/App/CrashPad.h"
#include "Core/Controller/Interface/InterfaceSelector.h"
#include "LeadingDelta/LeadingDelta.h"

#include "Math/App.h"

int run(int argc, const char **argv) {
    auto input = new R2toR :: LeadingDelta :: Builder( );

    InterfaceManager::getInstance().registerInterface(input->getInterface());
    auto prog = Simulation::App(argc, argv, Core::Simulation::VoidBuilder::Ptr(input));

    return prog.run();
}

int main(int argc, const char **argv) {
    return SafetyNet::jump(run, argc, argv);

}


