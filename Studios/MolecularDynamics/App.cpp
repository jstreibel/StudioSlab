//
// Created by joao on 8/6/23.
//

#include "App.h"

#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "../../Lib/Core/Controller/CommandLine/CommandLineArgsManager.h"
#include "Core/SlabCore.h"

#include "Graphics/SlabGraphics.h"

#define SFML_Backend dynamic_cast<Graphics::SFMLBackend&>(Graphics::GetGraphicsBackend())

namespace Studios::MolecularDynamics {

    static Slab::Models::MolecularDynamics::FRecipe Recipe;

    MolecularDynamics::App::App(int argc, const char **argv)
            : AppBase(argc, argv) {
        Slab::Core::BackendManager::Startup("SFML");

        Slab::Core::CLArgsManager::Parse(argc, argv);
    }

    int MolecularDynamics::App::run() {

        NumericTask = Slab::New<Slab::Math::FNumericTask>(Slab::Dummy(Recipe));

        auto taskManager = Slab::DynamicPointerCast<Slab::Core::MTaskManager>(Slab::Core::GetModule("TaskManager"));
        taskManager->AddTask(NumericTask);

        Slab::Graphics::GetGraphicsBackend()->Run();

        Slab::Core::Log::Info() << "MolecularDynamics finished." << Slab::Core::Log::Flush;

        NumericTask->Abort();

        return true;
    }

}