//
// Created by joao on 4/1/23.
//

#include "Core/App/CrashPad.h"

#include "Core/Controller/Interface/InterfaceSelector.h"
#include "Math/Function/Maps/RtoR/App/FieldsApp-RtoR.h"

#include "Signal.h"
#include "Core/Backend/GLUT/GLUTBackend.h"
#include "Core/Tools/Log.h"

auto main(int argc, const char **argv) -> int {

    try {
        auto &im = InterfaceSelector::getInstance();

        /* sim 0 */im.registerOption(new RtoR::Signal::CLI);

        im.preParse(argc, argv);

        auto *prog = new SimulationsAppRtoR(argc, argv);

        return SafetyNet::jump(*prog);
    }
    catch(const char *msg){
        Log::Fatal() << "Exception: " << msg << std::endl;
        return 2;
    }
}

