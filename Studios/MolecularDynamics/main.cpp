//
// Created by joao on 8/6/23.
//

#include "CrashPad.h"
#include "App.h"

int run(int argc, const char *argv[]){
    Studios::MolecularDynamics::App app(argc, argv);

    return app.run();
}

int main(int argc, const char *argv[]) {
    return Slab::SafetyNet::jump(run, argc, argv);
}