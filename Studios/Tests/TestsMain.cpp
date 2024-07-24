//
// Created by joao on 29/08/2021.
//


#include "TestsApp.h"
#include <Core/App/CrashPad.h>


int run(int argc, const char **argv){
    Slab::Core::AppBase *prog = new TestsApp(argc, argv);

    return prog->run();
}


int main(int argc, const char **argv) {
    return Slab::Core::SafetyNet::jump(run, argc, argv);
}


