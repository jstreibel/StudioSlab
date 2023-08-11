//
// Created by joao on 8/08/23.
//


#include "Stepper.h"
#include "Monitor.h"

#include "Builder.h"

#include "Base/Backend/SFML-Nuklear/SFML-Nuklear-Backend.h"
#include "Phys/Numerics/Output/Plugs/OutputConsoleMonitor.h"
#include "Base/Tools/Log.h"

#define DO_REGISTER true

namespace MolecularDynamics {
    Builder::Builder()
    : VoidBuilder("Molecular Dynamics", "Builder for 2-d molecular dynamics simulations", DO_REGISTER) {}

    OutputManager *Builder::buildOutputManager() {
        auto outputManager = new OutputManager(numericParams);

        outputManager->addOutputChannel(new OutputConsoleMonitor(numericParams, numericParams.getn()/5));

        auto monitor = new MolecularDynamics::Monitor(numericParams);
        Backend::GetInstanceSuper<GUIBackend>().addWindow(std::shared_ptr<Window>(monitor));
        outputManager->addOutputChannel(monitor);

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

    void Builder::notifyCLArgsSetupFinished() {
        InterfaceOwner::notifyCLArgsSetupFinished();

        Log::Attention("ParticleDynamics::Builder ") << "will ignore NumericParams '-t' argument and set it to negative.";

        numericParams.sett(-1);
    }

} // MolecularDynamics