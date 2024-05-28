//
// Created by joao on 8/08/23.
//

#ifndef STUDIOSLAB_MOLDYN_BUILDER_H
#define STUDIOSLAB_MOLDYN_BUILDER_H

#include "Math/Numerics/NumericalRecipe.h"

namespace MolecularDynamics {

    class Builder : public Core::Simulation::NumericalRecipe {
        RealParameter temperature = RealParameter    (0.0, "T,temperature", "The system temperature in Langevin simulations.");
        RealParameter dissipation = RealParameter    (0.0, "k,dissipation_factor", "The system energy dissipation factor.");
        IntegerParameter model    = IntegerParameter (  0, "model", "The physical model to use:"
                                                                    "\n\t\t0: Lennard-Jones"
                                                                    "\n\t\t1: Soft disk dynamics");

        Interface::Ptr molDynamicsInterface;

    public:
        Builder();

        auto buildOutputManager() -> OutputManager * override;

        auto buildEquationSolver() -> void * override;

        auto buildStepper() -> Stepper * override;

        auto getBoundary() -> void * override;

        auto getInitialState() -> void * override;

        auto newFunctionArbitrary() -> void * override;

        auto newFieldState() -> void * override;

        auto notifyCLArgsSetupFinished() -> void override;

    };

} // MolecularDynamics

#endif //STUDIOSLAB_BUILDER_H
