//
// Created by joao on 8/08/23.
//


#include "VerletStepper.cpp"
#include "VerletStepper.h"

#include "Hamiltonians/Lennard-Jones/LennardJones.h"
#include "Hamiltonians/SoftDisk/SoftDisk.h"

#include "Monitor.h"
#include "Builder.h"

#include "Base/Backend/SFML-Nuklear/SFML-Nuklear-Backend.h"
#include "Base/Controller/Interface/InterfaceManager.h"

#include "Phys/Numerics/Output/Plugs/OutputConsoleMonitor.h"
#include "Base/Tools/Log.h"


#define DO_REGISTER true
#define DONT_REGISTER (!DO_REGISTER)

namespace MolecularDynamics {
    Builder::Builder()
    : VoidBuilder("2D Molecular Dynamics", "Builder for 2-d molecular dynamics simulations", DONT_REGISTER)
    , molDynamicsInterface(Interface::New("Molecular dynamics 2-d", this, 100))
    {
        molDynamicsInterface->addParameters({&temperature, &dissipation, &model});
        interface->addSubInterface(molDynamicsInterface);
        InterfaceManager::getInstance().registerInterface(interface);
    }

    OutputManager *Builder::buildOutputManager() {
        auto outputManager = new OutputManager(numericParams);

        outputManager->addOutputChannel(new OutputConsoleMonitor(numericParams, numericParams.getn()/5));

        MolecularDynamics::Monitor::Model simModel = *model==0
                ? MolecularDynamics::Monitor::Model::LennardJones
                : MolecularDynamics::Monitor::Model::SoftDisk;
        auto monitor = new MolecularDynamics::Monitor(numericParams, simModel);
        Backend::GetInstanceSuper<GUIBackend>().addWindow(std::shared_ptr<Window>(monitor));
        outputManager->addOutputChannel(monitor);

        return outputManager;
    }

    Stepper *Builder::buildStepper() {
        fix T = *temperature;
        fix k = *dissipation;

        if (*model == 0) {
            LennardJones lj(numericParams);
            lj.setDissipation(k);
            lj.setTemperature(T);
            return new MolecularDynamics::VerletStepper<LennardJones>(numericParams, lj);
        }
        if (*model == 1) {
            SoftDisk sd(numericParams, 0);
            sd.setDissipation(k);
            sd.setTemperature(T);
            return new MolecularDynamics::VerletStepper<SoftDisk>(numericParams, sd);
        }

        throw Str("Unknown particle dynamics model '") + ToStr(*model) + "'.";
    }

    void *Builder::buildEquationSolver() {
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

    void Builder::notifyCLArgsSetupFinished() {
        InterfaceOwner::notifyCLArgsSetupFinished();

        Log::Attention("ParticleDynamics::Builder ") << "will ignore NumericParams '-t' argument and set it to negative.";

        numericParams.sett(-1);
    }

} // MolecularDynamics