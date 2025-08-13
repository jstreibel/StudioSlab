//
// Created by joao on 1/09/23.
//

#include "CrashPad.h"
#include "MathApp.h"

#include "Recipes/NumericalRecipe_Ak2.h"
#include "Recipes/NumericalRecipe_PlaneWaves.h"
#include "Recipes/NumericalRecipe_wkA.h"
#include "Recipes/Signal_Ak2_Recipe.h"

int run(int argc, const char *argv[]) {

#ifdef FIELDS_MODES_PlaneWaves
    auto builder = new Modes::FNumericalRecipe_PlaneWaves();
#elif FIELDS_MODES_wkA
    auto builder = new Modes::NumericalRecipe_wkA();
#elif defined FIELDS_SIGNAL_Ak2
    auto builder = new Modes::Signal_Ak2_Recipe();
#else
    auto builder = new Modes::NumericalRecipe_Ak2();
#endif

    using namespace Slab::Math;

    auto prog = MathApp(argc, argv, Slab::Models::KGRtoR::FKGRtoR_Recipe_ptr(builder));

    return prog.run();
}

int main(int argc, const char *argv[]){
    return Slab::SafetyNet::jump(run, argc, argv);
}