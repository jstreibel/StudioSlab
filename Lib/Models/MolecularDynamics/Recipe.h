//
// Created by joao on 8/08/23.
//

#ifndef STUDIOSLAB_MOLDYN_BUILDER_H
#define STUDIOSLAB_MOLDYN_BUILDER_H

#include "Core/Controller/CommandLine/CommonCLParameters.h"

#include "Math/Numerics/NumericalRecipe.h"
#include "Math/Numerics/Socket.h"

namespace Slab::Models::MolecularDynamics {

    using namespace Slab;
    using namespace Slab::Core;

    class Recipe : public Math::Base::NumericalRecipe {
        RealParameter temperature = RealParameter    (0.0, "T,temperature", "The system temperature in Langevin simulations.");
        RealParameter dissipation = RealParameter    (0.0, "k,dissipation_factor", "The system energy dissipation factor.");
        IntegerParameter model    = IntegerParameter (  0, "model", "The physical model to use:"
                                                                    "\n\t\t0: Lennard-Jones"
                                                                    "\n\t\t1: Soft disk dynamics");

        CLInterface_ptr molDynamicsInterface;

    public:
        Recipe();

        auto buildOutputSockets() -> Vector<Pointer<Math::Socket>> override;

        auto buildStepper() -> Pointer<Math::Stepper> override;

        auto notifyCLArgsSetupFinished() -> void override;

    };

} // MolecularDynamics

#endif //STUDIOSLAB_BUILDER_H
