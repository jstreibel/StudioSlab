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

    struct FRandomSite { UInt i; UInt j; };
    using RandomSite [[deprecated("Use FRandomSite")]] = FRandomSite;
    using FNewValue = DevFloat;
    using FR2toRMetropolis = FMetropolisAlgorithm<FRandomSite, FNewValue>;
    using NewValue [[deprecated("Use FNewValue")]] = FNewValue;
    using R2toRMetropolis [[deprecated("Use FR2toRMetropolis")]] = FR2toRMetropolis;

    class FR2toRMetropolisRecipe : public Base::FNumericalRecipe {
        TPointer<R2toR::NumericFunction_CPU> field_data;

        using Site = struct{UInt i; UInt j;};
        using FR2toRMetropolisSetup = FR2toRMetropolis::Setup;

    public:
        auto getField() -> TPointer<R2toR::NumericFunction_CPU>;

        explicit FR2toRMetropolisRecipe(UInt max_steps);

        auto BuildOutputSockets() -> Vector<TPointer<FOutputChannel>> override;

        auto BuildStepper() -> TPointer<FStepper> override;
    };

    using R2toRMetropolisRecipe [[deprecated("Use FR2toRMetropolisRecipe")]] = FR2toRMetropolisRecipe;

} // Slab::Math

#endif //STUDIOSLAB_R2TOR_METROPOLIS_RECIPE_H
