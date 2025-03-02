//
// Created by joao on 1/09/23.
//

#include "CrashPad.h"
#include "MathApp.h"
#include "NumericalRecipe_Ak2.h"
#include "NumericalRecipe_wkA.h"

int run(int argc, const char *argv[]) {

#ifdef FIELDS_MODES_wkA
    auto builder = new Modes::NumericalRecipe_wkA();
#else
    auto builder = new Modes::NumericalRecipe_Ak2();
#endif

    using namespace Slab::Math;

    auto prog = MathApp(argc, argv, Slab::Models::KGRtoR::KGRtoRBuilder_ptr(builder));

    return prog.run();
}

int main(int argc, const char *argv[]){
    return Slab::SafetyNet::jump(run, argc, argv);
}