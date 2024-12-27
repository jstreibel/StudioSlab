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
    using RtoRMetropolis = MetropolisAlgorithm<Int, RealPair>;
    struct FieldPair { Pointer<RtoR::NumericFunction_CPU> ϕ, π; };

    class RtoRHamiltonianMetropolisHastingsRecipe : public Base::NumericalRecipe {
        FieldPair field_data;
        using RtoRMetropolisSetup = MetropolisSetup<Int, RealPair>;

    public:
        explicit RtoRHamiltonianMetropolisHastingsRecipe(UInt max_steps);

        auto getField() -> FieldPair;

        auto buildOutputSockets() -> Vector<Pointer<Socket>> override;

        auto buildStepper() -> Pointer<Stepper> override;
    };

} // Slab::Math

#endif //STUDIOSLAB_RTOR_HAMILTONIAN_METROPOLISHASTINGS_RECIPE_H
