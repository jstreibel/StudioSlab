//
// Created by joao on 10/28/24.
//

#include "MetropolisR2toRRecipe.h"

namespace Slab::Math {
    Vector<Pointer<Socket>> MetropolisR2toRRecipe::buildOutputSockets() {

        return {};
    }

    Pointer<Stepper> MetropolisR2toRRecipe::buildStepper() {
        return Slab::Pointer<Stepper>();
    }
} // Slab::Math