//
// Created by joao on 8/6/23.
//

#include "App.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Core/Controller/CommandLine/CLArgsManager.h"
#include "Math/Numerics/NumericTask.h"
#include "Core/Backend/BackendManager.h"

#define SFML_Backend dynamic_cast<SFMLBackend&>(Slab::Core::BackendManager::GetGUIBackend())

using namespace Slab;

MolecularDynamics::Recipe recipe;

MolecularDynamics::App::App(int argc, const char **argv)
: AppBase(argc, argv)
{
    Core::BackendManager::Startup(Slab::Core::SFML);

    numericTask = Slab::New<Math::NumericTask> (recipe);


    CLArgsManager::Parse();
}

int MolecularDynamics::App::run() {

    auto taskManager = DynamicPointerCast<Core::TaskManagerModule>(Core::BackendManager::GetModule(Slab::Core::TaskManager));
    taskManager->addTask(numericTask);

    SFML_Backend.run();

    Log::Info() << "MolecularDynamics finished." << Log::Flush;

    return true;
}

