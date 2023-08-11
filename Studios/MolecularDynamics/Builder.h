//
// Created by joao on 8/08/23.
//

#ifndef STUDIOSLAB_BUILDER_H
#define STUDIOSLAB_BUILDER_H

#include "Phys/Numerics/VoidBuilder.h"

namespace MolecularDynamics {

    class Builder : public Base::Simulation::VoidBuilder {

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
