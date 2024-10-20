//
// Created by joao on 8/08/23.
//


#include "VerletStepper.cpp"
#include "VerletStepper.h"

#include "Hamiltonians/Lennard-Jones/LennardJones.h"
#include "Hamiltonians/SoftDisk/SoftDisk.h"

#include "Monitor.h"
#include "Recipe.h"

#include "Graphics/Backend/SFML/SFMLBackend.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"

#include "Math/Numerics/Output/Plugs/OutputConsoleMonitor.h"
#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"


namespace MolecularDynamics {
    Recipe::Recipe()
    : NumericalRecipe(New<Slab::Models::MolecularDynamics::MolDynNumericConfig>(), "2D Molecular Dynamics", "Builder for 2-d molecular dynamics simulations", false)
    , molDynamicsInterface(New <CLInterface> ("Molecular dynamics 2-d", this, 100))
    {
        molDynamicsInterface->addParameters({&temperature, &dissipation, &model});
        interface->addSubInterface(molDynamicsInterface);
        CLInterfaceManager::getInstance().registerInterface(interface);
    }

    Pointer<Math::OutputManager> Recipe::buildOutputManager() {
        auto numericConfig = DynamicPointerCast<Slab::Models::MolecularDynamics::MolDynNumericConfig>(numeric_config);

        auto outputManager = New <Math::OutputManager> (numeric_config->getn());

        outputManager->addOutputChannel(Slab::New <Slab::Math::OutputConsoleMonitor> (numericConfig->getn(),
                                                                                      numericConfig->gett()));

        MolecularDynamics::Monitor::Model simModel = *model==0
                ? MolecularDynamics::Monitor::Model::LennardJones
                : MolecularDynamics::Monitor::Model::SoftDisk;
        auto monitor = New <MolecularDynamics::Monitor>(numericConfig, simModel);
        Slab::Graphics::GetGraphicsBackend().addAndOwnEventListener(Pointer<Graphics::SystemWindowEventListener>(monitor));
        outputManager->addOutputChannel(monitor);

        return outputManager;
    }

    Pointer<Math::Stepper> Recipe::buildStepper() {
        auto c = DynamicPointerCast<Slab::Models::MolecularDynamics::MolDynNumericConfig>(numeric_config);

        fix T = *temperature;
        fix k = *dissipation;

        if (*model == 0) {
            LennardJones lj(c, T);
            lj.setDissipation(k);
            lj.setTemperature(T);
            return New <MolecularDynamics::VerletStepper<LennardJones>>(c, lj);
        }
        if (*model == 1) {
            SoftDisk sd(c, 0);
            sd.setDissipation(k);
            sd.setTemperature(T);
            return New<MolecularDynamics::VerletStepper<SoftDisk>>(c, sd);
        }

        throw Str("Unknown particle dynamics model '") + ToStr(*model) + "'.";
    }

    Math::Base::Solver_ptr Recipe::buildEquationSolver() {
        throw Str(__PRETTY_FUNCTION__) + " not implemented.";
    }

    void Recipe::notifyCLArgsSetupFinished() {
        CLInterfaceOwner::notifyCLArgsSetupFinished();

        Log::Attention("ParticleDynamics::Builder ") << "will ignore NumericParams '-t' argument and set it to negative.";

        DynamicPointerCast<Models::MolecularDynamics::MolDynNumericConfig>(getNumericConfig())->sett(-1);
    }

} // MolecularDynamics