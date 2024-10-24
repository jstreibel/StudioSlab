//
// Created by joao on 7/30/23.
//

#include "App.h"

#include <utility>

#include "Core/Backend/Backend.h"
#include "Core/Backend/BackendManager.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"

#include "Math/Numerics/NumericTask.h"
#include "Core/SlabCore.h"


namespace Slab::Math {

    App::App(int argc, const char **argv, Base::NumericalRecipe_ptr simBuilder)
            : AppBase(argc, argv), builder(std::move(simBuilder)) {
        parseCLArgs();
    }

    auto App::run() -> int {
        auto integrationTask = Slab::New<NumericTask>(*builder.get());
        auto &backend = Core::BackendManager::GetBackend();

        auto taskManager = dynamic_cast<Slab::Core::TaskManagerModule*>(Core::BackendManager::GetModule("TaskManager").get());

        taskManager->addTask(integrationTask);

        backend.run();

        // If tasks are still running after backend has finished around, we abort all tasks.
        taskManager->abortAllTasks();

        return 0;
    }

}