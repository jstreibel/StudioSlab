//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H
#define STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H

#include "Math/Numerics/NumericalRecipe.h"
#include "Math/Numerics/Metropolis/RtoR/RtoR-Metropolis-Config.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Metropolis-Setup.h"

namespace Slab::Math {

    class RtoRMetropolisRecipe : public Base::NumericalRecipe {
        Pointer<RtoR::NumericFunction_CPU> field_data;
        using RtoRMetropolisSetup = MetropolisSetup<UInt, Real>;

    public:
        auto getField() -> Pointer<RtoR::NumericFunction_CPU>;

        explicit RtoRMetropolisRecipe(UInt max_steps);

        auto buildOutputSockets() -> Vector<Pointer<Socket>> override;

        auto buildStepper() -> Pointer<Stepper> override;
    };

} // Slab::Math

#endif //STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H
