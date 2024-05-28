//
// Created by joao on 8/08/23.
//


#include "VerletStepper.cpp"
#include "VerletStepper.h"

#include "Hamiltonians/Lennard-Jones/LennardJones.h"
#include "Hamiltonians/SoftDisk/SoftDisk.h"

#include "Monitor.h"
#include "Builder.h"

#include "Core/Backend/SFML/SFMLBackend.h"
#include "Core/Controller/Interface/InterfaceManager.h"

#include "Math/Numerics/Output/Plugs/OutputConsoleMonitor.h"
#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"


#define DO_REGISTER true
#define DONT_REGISTER (!DO_REGISTER)

namespace MolecularDynamics {
    Builder::Builder()
    : NumericalRecipe("2D Molecular Dynamics", "Builder for 2-d molecular dynamics simulations", DONT_REGISTER)
    , molDynamicsInterface(Interface::New("Molecular dynamics 2-d", this, 100))
    {
        molDynamicsInterface->addParameters({&temperature, &dissipation, &model});
        interface->addSubInterface(molDynamicsInterface);
        InterfaceManager::getInstance().registerInterface(interface);
    }

    OutputManager *Builder::buildOutputManager() {
        auto &numericConfig = simulationConfig.numericConfig;

        auto outputManager = new OutputManager(numericConfig);

        outputManager->addOutputChannel(new OutputConsoleMonitor(numericConfig, numericConfig.getn() / 5));

        MolecularDynamics::Monitor::Model simModel = *model==0
                ? MolecularDynamics::Monitor::Model::LennardJones
                : MolecularDynamics::Monitor::Model::SoftDisk;
        auto monitor = new MolecularDynamics::Monitor(numericConfig, simModel);
        Core::BackendManager::GetGUIBackend().addEventListener(Core::GUIEventListener_ptr(monitor));
        outputManager->addOutputChannel(monitor);

        return outputManager;
    }

    Stepper *Builder::buildStepper() {
        auto &numericConfig = simulationConfig.numericConfig;

        fix T = *temperature;
        fix k = *dissipation;

        if (*model == 0) {
            LennardJones lj(numericConfig);
            lj.setDissipation(k);
            lj.setTemperature(T);
            return new MolecularDynamics::VerletStepper<LennardJones>(numericConfig, lj);
        }
        if (*model == 1) {
            SoftDisk sd(numericConfig, 0);
            sd.setDissipation(k);
            sd.setTemperature(T);
            return new MolecularDynamics::VerletStepper<SoftDisk>(numericConfig, sd);
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

        simulationConfig.numericConfig.sett(-1);
    }

} // MolecularDynamics