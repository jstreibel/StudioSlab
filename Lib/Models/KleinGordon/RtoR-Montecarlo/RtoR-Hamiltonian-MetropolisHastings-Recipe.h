//
// Created by joao on 24/12/24.
//

#ifndef STUDIOSLAB_RTOR_HAMILTONIAN_METROPOLISHASTINGS_RECIPE_H
#define STUDIOSLAB_RTOR_HAMILTONIAN_METROPOLISHASTINGS_RECIPE_H

#include "Math/Numerics/NumericalRecipe.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Math/Numerics/Metropolis/Metropolis-Setup.h"
#include "Math/Numerics/Metropolis/MetropolisAlgorithm.h"

namespace Slab::Models::KGRtoR::Metropolis {

    using namespace Slab::Math;

    // Site type is Int (not UInt) to help with periodic BC.
    using RtoRMetropolis = FMetropolisAlgorithm<Int, RealPair>;
    struct FFieldPair { TPointer<RtoR::NumericFunction_CPU> ϕ, π; };
    using FieldPair [[deprecated("Use FFieldPair")]] = FFieldPair;

    class FRtoRHamiltonianMetropolisHastingsRecipe : public Base::FNumericalRecipe {
        FFieldPair field_data;
        using RtoRMetropolisSetup = FMetropolisSetup<Int, RealPair>;

    public:
        explicit FRtoRHamiltonianMetropolisHastingsRecipe(UInt max_steps);

        auto getField() -> FFieldPair;

        auto BuildOutputSockets() -> Vector<TPointer<FOutputChannel>> override;

        auto BuildStepper() -> TPointer<FStepper> override;
    };

    using RtoRHamiltonianMetropolisHastingsRecipe [[deprecated("Use FRtoRHamiltonianMetropolisHastingsRecipe")]] =
            FRtoRHamiltonianMetropolisHastingsRecipe;

} // Slab::Math

#endif //STUDIOSLAB_RTOR_HAMILTONIAN_METROPOLISHASTINGS_RECIPE_H
