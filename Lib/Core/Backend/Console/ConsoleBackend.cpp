#include "ConsoleBackend.h"
#include "Core/Backend/BackendManager.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Core/Tools/Log.h"


namespace Slab::Core {


    FConsoleBackend::FConsoleBackend() : FBackend("Console backend") {
    }

    void FConsoleBackend::Run() {
        auto TaskManager = dynamic_cast<MTaskManager*>(BackendManager::GetModule("TaskManager").get());

        while(TaskManager->HasRunningTasks());
    }

    void FConsoleBackend::Terminate() {

    }

    FConsoleBackend::~FConsoleBackend() {
        Core::Log::Info() << "Destroying ConsoleBackend." << Log::Flush;
    }

}