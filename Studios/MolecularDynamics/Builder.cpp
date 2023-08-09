//
// Created by joao on 8/08/23.
//

#include "Stepper.h"

#include "Builder.h"

#include "Phys/Numerics/Output/Plugs/OutputConsoleMonitor.h"

#define DO_REGISTER true

namespace MolecularDynamics {
    Builder::Builder()
    : VoidBuilder("Molecular Dynamics", "Builder for 2-d molecular dynamics simulations", DO_REGISTER) {}

    OutputManager *Builder::buildOutputManager() {
        auto outputManager = new OutputManager(numericParams);

        outputManager->addOutputChannel(new OutputConsoleMonitor(numericParams, 20));

        return outputManager;
    }

    void *Builder::buildEquationSolver() {
        throw Str(__PRETTY_FUNCTION__) + " not implemented.";
    }

    Stepper *Builder::buildStepper() {
        return new MolecularDynamics::VerletStepper(numericParams);
    }

    void *Builder::getBoundary() {
        throw Str(__PRETTY_FUNCTION__) + " not implemented.";
    }

    void *Builder::getInitialState() {
        throw Str(__PRETTY_FUNCTION__) + " not implemented.";
    }

    void *Builder::newFunctionArbitrary() {
        throw Str(__PRETTY_FUNCTION__) + " not implemented.";
    }

    void *Builder::newFieldState() {
        throw Str(__PRETTY_FUNCTION__) + " not implemented.";
    }

} // MolecularDynamics