//
// Created by joao on 8/6/23.
//

#include "Base/App/CrashPad.h"
#include "App.h"

int run(int argc, const char *argv[]){
    MolecularDynamics::App app(argc, argv);
}

int main(int argc, const char *argv[]) {
    return SafetyNet::jump(run, argc, argv);
}