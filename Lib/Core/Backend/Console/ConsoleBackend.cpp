#include "ConsoleBackend.h"
#include "Core/Backend/BackendManager.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Core/Tools/Log.h"


namespace Slab::Core {


    ConsoleBackend::ConsoleBackend() : Backend("Console backend") {
    }

    void ConsoleBackend::run() {
        auto taskManager = dynamic_cast<TaskManagerModule*>(BackendManager::GetModule(Modules::TaskManager).get());

        while(taskManager->hasRunningTasks());
    }

    void ConsoleBackend::terminate() {

    }

    ConsoleBackend::~ConsoleBackend() {
        Core::Log::Info() << "Destroying ConsoleBackend." << Log::Flush;
    }

}