//
// Created by joao on 10/28/24.
//

#include "MetropolisR2toRRecipe.h"

namespace Slab::Math {
    Pointer<OutputManager> MetropolisR2toRRecipe::buildOutputManager() {

        return {};
    }

    Pointer<Stepper> MetropolisR2toRRecipe::buildStepper() {
        return Slab::Pointer<Stepper>();
    }
} // Slab::Math