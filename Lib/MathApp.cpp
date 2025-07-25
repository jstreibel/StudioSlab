//
// Created by joao on 7/30/23.
//

#include "MathApp.h"

#include <utility>

#include "Core/Backend/BackendManager.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"

#include "Math/Numerics/NumericTask.h"
#include "Core/Controller/CommandLine/CommandLineArgsManager.h"
#include "Graphics/Modules/GraphicsModule.h"


namespace Slab::Math {

    MathApp::MathApp(int argc, const char **argv, TPointer<Base::NumericalRecipe> SimBuilder)
            : AppBase(argc, argv), Recipe(std::move(SimBuilder)) {

        Core::CLArgsManager::Parse(argc, argv);
    }

    auto MathApp::run() -> int {
        const auto integrationTask = Slab::New<NumericTask>(Recipe);

        const auto TaskManager = dynamic_cast<Core::MTaskManager*>
                (Core::BackendManager::GetModule("TaskManager").get());

        TaskManager->AddTask(integrationTask);

        Core::BackendManager::GetBackend()->Run();

        // If tasks are still running after backend has finished around, we abort all tasks.
        TaskManager->AbortAllTasks();

        return 0;
    }

}
