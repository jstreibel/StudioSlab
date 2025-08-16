//
// Created by joao on 8/08/23.
//


#include "VerletStepper.cpp"
#include "VerletStepper.h"

#include "Hamiltonians/Lennard-Jones/LennardJones.h"
#include "Hamiltonians/SoftDisk/SoftDisk.h"

#include "Monitor.h"
#include "Recipe.h"

#include "Core/SlabCore.h"
#include "../../Core/Controller/CommandLineInterfaceManager.h"
#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"

#include "Graphics/SlabGraphics.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputConsoleMonitor.h"

#include "Graphics/Window/SlabWindowManager.h"

namespace Slab::Models::MolecularDynamics {
    Recipe::Recipe()
    : FNumericalRecipe(New<Slab::Models::MolecularDynamics::MolDynNumericConfig>(), "2D Molecular Dynamics", "Builder for 2-d molecular dynamics simulations", false)
    , molDynamicsInterface(New <FCommandLineInterface> ("Molecular dynamics 2-d", this, 100))
    {
        molDynamicsInterface->AddParameters({&temperature, &dissipation, &model});
        Interface->AddSubInterface(molDynamicsInterface);
        Core::RegisterCLInterface(Interface);
    }

    Vector<TPointer<Math::Socket>> Recipe::BuildOutputSockets() {
        Vector<TPointer<Math::Socket>> sockets;

        auto numericConfig = DynamicPointerCast<Slab::Models::MolecularDynamics::MolDynNumericConfig>(NumericConfig);

        sockets.emplace_back(Slab::New <Slab::Math::OutputConsoleMonitor> (numericConfig->getn()));

        MolecularDynamics::Monitor::Model simModel = *model==0
                ? MolecularDynamics::Monitor::Model::LennardJones
                : MolecularDynamics::Monitor::Model::SoftDisk;
        auto monitor = New <MolecularDynamics::Monitor>(numericConfig, simModel);

        const auto MainPlatformWindow = Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();

        const auto WindowManager = New<Graphics::FSlabWindowManager>();
        MainPlatformWindow->AddAndOwnEventListener(WindowManager);

        WindowManager->AddSlabWindow(TPointer<Graphics::FSlabWindow>(monitor), false);
        sockets.emplace_back(monitor);

        return sockets;
    }

    TPointer<Math::Stepper> Recipe::buildStepper() {
        auto c = DynamicPointerCast<Slab::Models::MolecularDynamics::MolDynNumericConfig>(NumericConfig);

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

        throw Exception(Str("Unknown particle dynamics model '") + ToStr(*model) + "'.");
    }

    void Recipe::NotifyCLArgsSetupFinished() {
        FCommandLineInterfaceOwner::NotifyCLArgsSetupFinished();

        Log::Attention("ParticleDynamics::Builder ") << "will ignore NumericParams '-t' argument and set it to negative.";

        DynamicPointerCast<Models::MolecularDynamics::MolDynNumericConfig>(GetNumericConfig())->sett(-1);
    }

} // MolecularDynamics