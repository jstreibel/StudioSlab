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

    using FRtoRRandomSite = UInt;
    using FRtoRNewValue = DevFloat;
    using FRtoRMetropolis = FMetropolisAlgorithm<FRtoRRandomSite, FRtoRNewValue>;
    using RtoRMetropolis [[deprecated("Use FRtoRMetropolis")]] = FRtoRMetropolis;

    class FRtoRActionMetropolisRecipe : public Base::FNumericalRecipe {
        TPointer<RtoR::NumericFunction_CPU> field_data;
        using FRtoRMetropolisSetup = FMetropolisSetup<FRtoRRandomSite, FRtoRNewValue>;

    public:
        auto getField() -> TPointer<RtoR::NumericFunction_CPU>;

        explicit FRtoRActionMetropolisRecipe(UInt max_steps);

        auto BuildOutputSockets() -> Vector<TPointer<FOutputChannel>> override;

        auto BuildStepper() -> TPointer<FStepper> override;
    };

    using RtoRActionMetropolisRecipe [[deprecated("Use FRtoRActionMetropolisRecipe")]] = FRtoRActionMetropolisRecipe;

} // Slab::Math

#endif //STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H
