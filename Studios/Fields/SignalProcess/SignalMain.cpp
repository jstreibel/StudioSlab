//
// Created by joao on 4/1/23.
//

#include "Studios/App/CrashPad.h"

#include "Studios/Controller/Interface/InterfaceSelector.h"
#include "Fields/Mappings/RtoR/App/FieldsApp-RtoR.h"

#include "Signal.h"
#include "Studios/Backend/GLUT/GLUTBackend.h"

auto main(int argc, const char **argv) -> int {

    try {
        auto &im = InterfaceSelector::getInstance();

        /* sim 0 */im.registerOption(new RtoR::Signal::CLI);

        im.preParse(argc, argv);

        auto *prog = new SimulationsAppRtoR(argc, argv);

        return SafetyNet::jump(*prog);
    }
    catch(const char *msg){
        std::cout << "Exception: " << msg << std::endl;
        return 2;
    }
}

