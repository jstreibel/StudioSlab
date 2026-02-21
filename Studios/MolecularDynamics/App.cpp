//
// Created by joao on 8/6/23.
//

#include "App.h"

#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "../../Lib/Core/Controller/CommandLine/CommandLineArgsManager.h"
#include "Core/SlabCore.h"

#include "Graphics/SlabGraphics.h"

#define SFML_Backend dynamic_cast<Graphics::FSFMLBackend&>(Graphics::GetGraphicsBackend())

namespace Studios::MolecularDynamics {

    static Slab::Models::MolecularDynamics::FMolecularDynamicsRecipe Recipe;

    MolecularDynamics::App::App(int argc, const char **argv)
            : FAppBase(argc, argv) {
        Slab::Core::FBackendManager::Startup("SFML");

        Slab::Core::FCLArgsManager::Parse(argc, argv);
    }

    int MolecularDynamics::App::run() {

        NumericTask = Slab::New<Slab::Math::FNumericTask>(Slab::Dummy(Recipe));

        auto taskManager = Slab::DynamicPointerCast<Slab::Core::FTaskManager>(Slab::Core::GetModule("TaskManager"));
        taskManager->AddTask(NumericTask);

        Slab::Graphics::GetGraphicsBackend()->Run();

        Slab::Core::Log::Info() << "MolecularDynamics finished." << Slab::Core::Log::Flush;

        NumericTask->Abort();

        return true;
    }

}
