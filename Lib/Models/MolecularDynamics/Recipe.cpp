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
#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"

#include "Graphics/SlabGraphics.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputConsoleMonitor.h"

#include "Graphics/Window/SlabWindowManager.h"

namespace Slab::Models::MolecularDynamics {
    Recipe::Recipe()
    : NumericalRecipe(New<Slab::Models::MolecularDynamics::MolDynNumericConfig>(), "2D Molecular Dynamics", "Builder for 2-d molecular dynamics simulations", false)
    , molDynamicsInterface(New <CLInterface> ("Molecular dynamics 2-d", this, 100))
    {
        molDynamicsInterface->addParameters({&temperature, &dissipation, &model});
        interface->addSubInterface(molDynamicsInterface);
        Core::RegisterCLInterface(interface);
    }

    Vector<Pointer<Math::Socket>> Recipe::buildOutputSockets() {
        Vector<Pointer<Math::Socket>> sockets;

        auto numericConfig = DynamicPointerCast<Slab::Models::MolecularDynamics::MolDynNumericConfig>(numeric_config);

        sockets.emplace_back(Slab::New <Slab::Math::OutputConsoleMonitor> (numericConfig->getn()));

        MolecularDynamics::Monitor::Model simModel = *model==0
                ? MolecularDynamics::Monitor::Model::LennardJones
                : MolecularDynamics::Monitor::Model::SoftDisk;
        auto monitor = New <MolecularDynamics::Monitor>(numericConfig, simModel);

        auto main_syswin = Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();

        auto wm = New<Graphics::SlabWindowManager>(main_syswin.get());
        wm->addSlabWindow(Pointer<Graphics::SlabWindow>(monitor));
        main_syswin->addAndOwnEventListener(wm);
        sockets.emplace_back(monitor);

        return sockets;
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

        throw Exception(Str("Unknown particle dynamics model '") + ToStr(*model) + "'.");
    }

    void Recipe::notifyCLArgsSetupFinished() {
        CLInterfaceOwner::notifyCLArgsSetupFinished();

        Log::Attention("ParticleDynamics::Builder ") << "will ignore NumericParams '-t' argument and set it to negative.";

        DynamicPointerCast<Models::MolecularDynamics::MolDynNumericConfig>(getNumericConfig())->sett(-1);
    }

} // MolecularDynamics