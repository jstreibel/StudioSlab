//
// Created by joao on 29/08/2021.
//


#include "TestsApp.h"
#include <Base/App/CrashPad.h>


int main(int argc, const char **argv) {
    AppBase *prog = new TestsApp(argc, argv);

    return SafetyNet::jump(*prog);
}


