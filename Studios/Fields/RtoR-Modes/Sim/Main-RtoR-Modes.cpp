//
// Created by joao on 1/09/23.
//

#include "Core/App/CrashPad.h"
#include "Math/App.h"
#include "Builder.h"

int run(int argc, const char *argv[]) {

    auto builder = new Modes::Builder();

    using namespace Slab::Math;

    auto prog = App(argc, argv, RtoR::KGBuilder::Ptr(builder));

    return prog.run();
}

int main(int argc, const char *argv[]){
    return Slab::Core::SafetyNet::jump(run, argc, argv);
}