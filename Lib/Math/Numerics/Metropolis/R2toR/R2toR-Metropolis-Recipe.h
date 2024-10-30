//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H
#define STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H

#include "Math/Numerics/NumericalRecipe.h"
#include "Math/Numerics/Metropolis/R2toR/R2toR-Metropolis-Config.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"

namespace Slab::Math {

    class R2toRMetropolisRecipe : public Base::NumericalRecipe {
        Pointer<R2toR::NumericFunction_CPU> field_data;

    public:
        auto getField() -> Pointer<R2toR::NumericFunction_CPU>;

        explicit R2toRMetropolisRecipe(UInt max_steps);

        auto buildOutputSockets() -> Vector<Pointer<Socket>> override;

        auto buildStepper() -> Pointer<Stepper> override;
    };

} // Slab::Math

#endif //STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H
