//
// Created by joao on 8/6/23.
//

#include "App.h"

#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Core/Controller/CommandLine/CLArgsManager.h"
#include "Core/SlabCore.h"

#include "Graphics/SlabGraphics.h"

#define SFML_Backend dynamic_cast<Graphics::SFMLBackend&>(Graphics::GetGraphicsBackend())

namespace Studios::MolecularDynamics {

    Slab::Models::MolecularDynamics::Recipe recipe;

    MolecularDynamics::App::App(int argc, const char **argv)
            : AppBase(argc, argv) {
        Slab::Core::BackendManager::Startup("SFML");

        numericTask = Slab::New<Slab::Math::NumericTask>(recipe);


        Slab::Core::CLArgsManager::Parse(argc, argv);
    }

    int MolecularDynamics::App::run() {

        auto taskManager = Slab::DynamicPointerCast<Slab::Core::TaskManagerModule>(Slab::Core::GetModule("TaskManager"));
        taskManager->addTask(numericTask);

        Slab::Graphics::GetGraphicsBackend().run();

        Slab::Core::Log::Info() << "MolecularDynamics finished." << Slab::Core::Log::Flush;

        return true;
    }

}