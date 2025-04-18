//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H
#define STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H

#include "Math/Numerics/NumericalRecipe.h"
#include "Math/Numerics/Metropolis/Metropolis-Config.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Numerics/Metropolis/Metropolis-Setup.h"
#include "Math/Numerics/Metropolis/MetropolisAlgorithm.h"

namespace Slab::Math {

    struct RandomSite{ UInt i; UInt j; };
    using NewValue = Real;
    using R2toRMetropolis = MetropolisAlgorithm<RandomSite, NewValue>;

    class R2toRMetropolisRecipe : public Base::NumericalRecipe {
        Pointer<R2toR::NumericFunction_CPU> field_data;

        using Site = struct{UInt i; UInt j;};
        using R2toRMetropolisSetup = R2toRMetropolis::Setup;

    public:
        auto getField() -> Pointer<R2toR::NumericFunction_CPU>;

        explicit R2toRMetropolisRecipe(UInt max_steps);

        auto buildOutputSockets() -> Vector<Pointer<Socket>> override;

        auto buildStepper() -> Pointer<Stepper> override;
    };

} // Slab::Math

#endif //STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H
