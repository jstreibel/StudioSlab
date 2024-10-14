//
// Created by joao on 8/08/23.
//


#include "VerletStepper.cpp"
#include "VerletStepper.h"

#include "Hamiltonians/Lennard-Jones/LennardJones.h"
#include "Hamiltonians/SoftDisk/SoftDisk.h"

#include "Monitor.h"
#include "Recipe.h"

#include "Core/Backend/SFML/SFMLBackend.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"

#include "Math/Numerics/Output/Plugs/OutputConsoleMonitor.h"
#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"


namespace MolecularDynamics {
    Recipe::Recipe()
    : NumericalRecipe("2D Molecular Dynamics", "Builder for 2-d molecular dynamics simulations", false)
    , molDynamicsInterface(New <CLInterface> ("Molecular dynamics 2-d", this, 100))
    {
        molDynamicsInterface->addParameters({&temperature, &dissipation, &model});
        interface->addSubInterface(molDynamicsInterface);
        CLInterfaceManager::getInstance().registerInterface(interface);
    }

    Pointer<Math::OutputManager> Recipe::buildOutputManager() {
        auto &numericConfig = simulationConfig.numericConfig;

        auto outputManager = New <Math::OutputManager> (numericConfig);

        outputManager->addOutputChannel(Slab::New <Slab::Math::OutputConsoleMonitor> (numericConfig));

        MolecularDynamics::Monitor::Model simModel = *model==0
                ? MolecularDynamics::Monitor::Model::LennardJones
                : MolecularDynamics::Monitor::Model::SoftDisk;
        auto monitor = New <MolecularDynamics::Monitor>(numericConfig, simModel);
        Core::BackendManager::GetGUIBackend().addAndOwnEventListener(Core::GUIEventListener_ptr(monitor));
        outputManager->addOutputChannel(monitor);

        return outputManager;
    }

    Pointer<Math::Stepper> Recipe::buildStepper() {
        auto &numericConfig = simulationConfig.numericConfig;

        fix T = *temperature;
        fix k = *dissipation;

        if (*model == 0) {
            LennardJones lj(numericConfig);
            lj.setDissipation(k);
            lj.setTemperature(T);
            return New <MolecularDynamics::VerletStepper<LennardJones>>(numericConfig, lj);
        }
        if (*model == 1) {
            SoftDisk sd(numericConfig, 0);
            sd.setDissipation(k);
            sd.setTemperature(T);
            return New<MolecularDynamics::VerletStepper<SoftDisk>>(numericConfig, sd);
        }

        throw Str("Unknown particle dynamics model '") + ToStr(*model) + "'.";
    }

    Math::Base::Solver_ptr Recipe::buildEquationSolver() {
        throw Str(__PRETTY_FUNCTION__) + " not implemented.";
    }

    void Recipe::notifyCLArgsSetupFinished() {
        CLInterfaceOwner::notifyCLArgsSetupFinished();

        Log::Attention("ParticleDynamics::Builder ") << "will ignore NumericParams '-t' argument and set it to negative.";

        simulationConfig.numericConfig.sett(-1);
    }

} // MolecularDynamics