//
// Created by joao on 8/08/23.
//


#include "VerletStepper.cpp"
#include "VerletStepper.h"

#include "Hamiltonians/Lennard-Jones/LennardJones.h"
#include "Hamiltonians/SoftDisk/SoftDisk.h"

#include "Monitor.h"
#include "MolecularDynamicsRecipe.h"

#include "Core/SlabCore.h"
#include "../../Core/Controller/InterfaceManager.h"
#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"

#include "Graphics/SlabGraphics.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputConsoleMonitor.h"

#include "Graphics/Window/SlabWindowManager.h"

namespace Slab::Models::MolecularDynamics {
    FMolecularDynamicsRecipe::FMolecularDynamicsRecipe()
    : FNumericalRecipe(New<MolDynNumericConfig>(false), "2D Molecular Dynamics", "Builder for 2-d molecular dynamics simulations", false)
    {
        Interface->AddParameters({&Temperature, &Dissipation, &Model});

        RegisterToManager();
    }

    Vector<TPointer<Math::FOutputChannel>> FMolecularDynamicsRecipe::BuildOutputSockets() {
        Vector<TPointer<Math::FOutputChannel>> sockets;

        auto numericConfig = DynamicPointerCast<Slab::Models::MolecularDynamics::MolDynNumericConfig>(NumericConfig);

        sockets.emplace_back(Slab::New <Slab::Math::OutputConsoleMonitor> (numericConfig->Get_n()));

        MolecularDynamics::Monitor::Model simModel = *Model==0
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

    TPointer<Math::FStepper> FMolecularDynamicsRecipe::BuildStepper() {
        auto c = DynamicPointerCast<Slab::Models::MolecularDynamics::MolDynNumericConfig>(NumericConfig);

        fix T = *Temperature;
        fix k = *Dissipation;

        if (*Model == 0) {
            LennardJones lj(c, T);
            lj.setDissipation(k);
            lj.setTemperature(T);
            return New <MolecularDynamics::TVerletStepper<LennardJones>>(c, lj);
        }
        if (*Model == 1) {
            SoftDisk sd(c, 0);
            sd.setDissipation(k);
            sd.setTemperature(T);
            return New<MolecularDynamics::TVerletStepper<SoftDisk>>(c, sd);
        }

        throw Exception(Str("Unknown particle dynamics model '") + ToStr(*Model) + "'.");
    }

    void FMolecularDynamicsRecipe::NotifyInterfaceSetupIsFinished() {
        FInterfaceOwner::NotifyInterfaceSetupIsFinished();

        // Log::Attention() << "ParticleDynamics::Builder will ignore NumericParams '-t' argument and set it to negative.";
        // DynamicPointerCast<MolDynNumericConfig>(GetNumericConfig())->Set_t(-1);
    }

} // MolecularDynamics