//
// Created by joao on 10/8/21.
//

#include "Base/App/CrashPad.h"
#include "Base/Controller/Interface/InterfaceSelector.h"
#include "LeadingDelta/LeadingDelta.h"

#include "Phys/App.h"

int run(int argc, const char **argv) {
    auto input = new R2toR :: LeadingDelta :: Builder( );

    InterfaceManager::getInstance().registerInterface(input->getInterface());
    auto prog = Simulation::App(argc, argv, Base::Simulation::VoidBuilder::Ptr(input));

    return prog.run();
}

int main(int argc, const char **argv) {
    return SafetyNet::jump(run, argc, argv);

}


