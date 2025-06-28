//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H
#define STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H

#include "Math/Numerics/NumericalRecipe.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Math/Numerics/Metropolis/Metropolis-Setup.h"
#include "Math/Numerics/Metropolis/MetropolisAlgorithm.h"

namespace Slab::Math {

    using RtoRMetropolis = MetropolisAlgorithm<UInt, DevFloat>;

    class RtoRActionMetropolisRecipe : public Base::NumericalRecipe {
        Pointer<RtoR::NumericFunction_CPU> field_data;
        using RtoRMetropolisSetup = MetropolisSetup<UInt, DevFloat>;

    public:
        auto getField() -> Pointer<RtoR::NumericFunction_CPU>;

        explicit RtoRActionMetropolisRecipe(UInt max_steps);

        auto buildOutputSockets() -> Vector<Pointer<Socket>> override;

        auto buildStepper() -> Pointer<Stepper> override;
    };

} // Slab::Math

#endif //STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H
