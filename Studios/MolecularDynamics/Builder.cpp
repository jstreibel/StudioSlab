//
// Created by joao on 8/08/23.
//

#include "Builder.h"

namespace MolecularDynamics {
    Builder::Builder()
    : VoidBuilder("Molecular Dynamics", "Builder for 2-d molecular dynamics simulations") {}

    OutputManager *Builder::buildOutputManager() {
        throw Str(__PRETTY_FUNCTION__) + " not implemented.";
    }

    void *Builder::buildEquationSolver() {
        throw Str(__PRETTY_FUNCTION__) + " not implemented.";
    }

    Stepper *Builder::buildStepper() {
        throw Str(__PRETTY_FUNCTION__) + " not implemented.";
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