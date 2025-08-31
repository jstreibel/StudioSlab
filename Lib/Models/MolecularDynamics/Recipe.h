//
// Created by joao on 8/08/23.
//

#ifndef STUDIOSLAB_MOLDYN_BUILDER_H
#define STUDIOSLAB_MOLDYN_BUILDER_H

#include "../../Core/Controller/Parameter/BuiltinParameters.h"

#include "Math/Numerics/NumericalRecipe.h"
#include "Math/Numerics/OutputChannel.h"

namespace Slab::Models::MolecularDynamics {

    using namespace Slab;
    using namespace Slab::Core;

    class Recipe : public Math::Base::FNumericalRecipe {
        RealParameter temperature = RealParameter    (0.0, FParameterDescription{'T', "temperature", "The system temperature in Langevin simulations."});
        RealParameter dissipation = RealParameter    (0.0, FParameterDescription{'k', "dissipation_factor", "The system energy dissipation factor."});
        IntegerParameter model    = IntegerParameter (  0, FParameterDescription{"model", "The physical model to use:"
                                                                                          "\n\t\t0: Lennard-Jones"
                                                                                          "\n\t\t1: Soft disk dynamics"});

        FInterface_ptr molDynamicsInterface;

    public:
        Recipe();

        auto BuildOutputSockets() -> Vector<TPointer<Math::FOutputChannel>> override;

        auto buildStepper() -> TPointer<Math::FStepper> override;

        auto NotifyInterfaceSetupIsFinished() -> void override;

    };

} // MolecularDynamics

#endif //STUDIOSLAB_MOLDYN_BUILDER_H
