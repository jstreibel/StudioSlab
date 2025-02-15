//
// Created by joao on 1/09/23.
//

#include "CrashPad.h"
#include "MathApp.h"
#include "NumericalRecipe.h"

int run(int argc, const char *argv[]) {

    auto builder = new Modes::NumericalRecipe();

    using namespace Slab::Math;

    auto prog = MathApp(argc, argv, Slab::Models::KGRtoR::KGRtoRBuilder_ptr(builder));

    return prog.run();
}

int main(int argc, const char *argv[]){
    return Slab::SafetyNet::jump(run, argc, argv);
}