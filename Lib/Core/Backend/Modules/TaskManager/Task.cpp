//
// Created by joao on 6/10/24.
//

#include "Task.h"
#include "Core/Tools/Log.h"

#include <utility>

namespace Slab::Core {

    FTask::FTask(Str name) : Name(std::move(name)), bContinueFlag(false) {
    }

    void FTask::Start() {
        TaskStatus = TaskRunning;
        ThreadStatus = ThreadRunning;

        try {
            TaskStatus = Run();
        } catch (const std::exception &e) {
            TaskStatus = TaskError;
            FLog::Error() << "Task \"" << Name << "\" threw exception: " << e.what() << FLog::Flush;
        } catch (...) {
            TaskStatus = TaskError;
            FLog::Error() << "Task \"" << Name << "\" threw unknown exception." << FLog::Flush;
        }

        std::unique_lock lock(FinishMutex);
        ThreadStatus = ThreadWaitingRelease;
        ReleaseCondition.wait(lock, [this] { return bContinueFlag; });
        ThreadStatus = ThreadFinished;
    }

    bool FTask::IsTaskRunning() const { return TaskStatus==TaskRunning; }

    bool FTask::IsThreadRunning() const
    {
        NOT_IMPLEMENTED_CLASS_METHOD
    }

    Str FTask::GetName() const { return Name; }

    ETaskStatus FTask::GetStatus() const {
        return TaskStatus;
    }

    void FTask::Release() {
        {
            std::lock_guard lock(FinishMutex);
            bContinueFlag = true;
        }
        ReleaseCondition.notify_all();
    }
}
