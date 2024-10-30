//
// Created by joao on 7/30/23.
//

#include "MathApp.h"

#include <utility>

#include "Core/Backend/BackendManager.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"

#include "Math/Numerics/NumericTask.h"
#include "Core/Controller/CommandLine/CLArgsManager.h"


namespace Slab::Math {

    MathApp::MathApp(int argc, const char **argv, Base::NumericalRecipe_ptr simBuilder)
            : AppBase(argc, argv), builder(std::move(simBuilder)) {
        Core::CLArgsManager::Parse(argc, argv);
    }

    auto MathApp::run() -> int {
        auto integrationTask = Slab::New<NumericTask>(*builder.get());

        auto taskManager = dynamic_cast<Slab::Core::TaskManagerModule*>
                (Core::BackendManager::GetModule("TaskManager").get());

        taskManager->addTask(integrationTask);

        Core::BackendManager::GetBackend().run();

        // If tasks are still running after backend has finished around, we abort all tasks.
        taskManager->abortAllTasks();

        return 0;
    }

}