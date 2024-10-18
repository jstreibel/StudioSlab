//
// Created by joao on 8/6/23.
//

#include "App.h"

#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Core/Controller/CommandLine/CLArgsManager.h"
#include "Core/SlabCore.h"

#include "Graphics/SlabGraphics.h"

#define SFML_Backend dynamic_cast<Graphics::SFMLBackend&>(Graphics::GetGraphicsBackend())

using namespace Slab;

MolecularDynamics::Recipe recipe;

MolecularDynamics::App::App(int argc, const char **argv)
: AppBase(argc, argv)
{
    Core::BackendManager::Startup("SFML");

    numericTask = Slab::New<Math::NumericTask> (recipe);


    CLArgsManager::Parse();
}

int MolecularDynamics::App::run() {

    auto taskManager = DynamicPointerCast<Core::TaskManagerModule>(Slab::Core::GetModule("TaskManager"));
    taskManager->addTask(numericTask);

    SFML_Backend.run();

    Log::Info() << "MolecularDynamics finished." << Log::Flush;

    return true;
}

