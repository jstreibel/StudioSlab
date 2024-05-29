//
// Created by joao on 7/30/23.
//

#include "App.h"

#include <utility>

#include "Core/Backend/Backend.h"
#include "Core/Backend/BackendManager.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"

#include "Math/Numerics/NumericTask.h"


namespace Slab::Math {

    App::App(int argc, const char **argv, Base::NumericalRecipe_ptr simBuilder)
            : AppBase(argc, argv), builder(std::move(simBuilder)) {
        parseCLArgs();
    }

    auto App::run() -> int {
        auto integrationTask = Slab::New<NumericTask>(*builder.get());
        auto &backend = Core::BackendManager::GetBackend();

        auto taskManager = dynamic_cast<Slab::Core::TaskManagerModule*>(BackendManager::GetModule(Modules::TaskManager).get());

        taskManager->addTask(integrationTask);

        backend.run();

        return 0;
    }

}