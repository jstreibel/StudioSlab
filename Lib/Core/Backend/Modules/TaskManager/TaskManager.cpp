//
// Created by joao on 23/05/24.
//

#include "TaskManager.h"
#include "Core/Tools/Log.h"

namespace Slab::Core {

    FTaskManager::FTaskManager(FTaskManager::EDestructorPolicy Policy)
    : FSlabModule("Task Manager"), destructorPolicy(Policy) {    }

    FTaskManager::~FTaskManager() {
        if(destructorPolicy == WaitAll) {
            for(const auto & [Task, JobThread] : Jobs) {
                if (JobThread->joinable()) JobThread->join();
            }
        }
        else if(destructorPolicy == AbortAll)
            AbortAllTasks();
    }

    auto FTaskManager::AddTask(const FTaskPointer& Task) -> FTaskManager::FJob {
        auto funky = [Task]() {
            Task->Start();

            switch (Task->GetStatus()) {
                case TaskNotInitialized:
                case TaskRunning:
                    // Task should not be running after finished...
                    FLog::Error() << "Job \"" << Task->GetName() << "\" finished with unexpected status." << FLog::Flush;
                    break;
                case TaskSuccess:
                    FLog::Success() << "Finished job \"" << Task->GetName() << "\"." << FLog::Flush;
                    break;
                case TaskError:
                    FLog::Fail() << "Job \"" << Task->GetName() << "\" failed (internal task error)." << FLog::Flush;
                    break;
                case TaskAborted:
                    FLog::Warning() << "Job \"" << Task->GetName() << "\" aborted." << FLog::Flush;
                    break;
            }
        };

        auto thread = New<std::thread>(funky);
        FLog::Critical() << "Started job \"" << Task->GetName() << "\" on thread "
                        << "[id " << thread->get_id() << "] [handle " << thread->native_handle() << "] " << FLog::Flush;
        auto job = FJob(Task, thread);

        {
            std::lock_guard lock(AddJobMutex); // make sure things go smooth on adding tasks
            Jobs.emplace_back(job);
        }

        return job;
    }

    void FTaskManager::AbortAllTasks() const
    {
        for(const auto& job : Jobs)
            Abort(job);
    }

    void FTaskManager::Abort(const FJob& Job) {
        auto &Task = Job.Task;
        auto &Thread = Job.JobThread;

        Task->Release();

        if(Task->IsTaskRunning()) {
            Task->Abort();
            FLog::Info() << "Sent abort signal to task \"" << Task->GetName() << "\"." << FLog::Flush;
        }

        if(Thread->joinable()) {
            FLog::Status() << "Waiting for task \"" << Task->GetName() << "\" thread to join...";
            Thread->join();
            FLog::Success() << "Task \"" << Task->GetName() << "\" thread has joined main thread.";
        }

    }

    auto FTaskManager::HasRunningTasks() const -> bool {
        return std::ranges::any_of(Jobs, [](const FJob &job){ return job.Task->IsTaskRunning(); });
    }

    auto FTaskManager::GetNumberOfRunningTasks() const -> size_t
    {
        return Jobs.size();
    }

    TList<FTaskManager::FJob> FTaskManager::GetAllJobs()
    {
        return Jobs;
    }

    bool FTaskManager::ClearJob(const FJob& Job)
    {
        if (Job.Task->IsTaskRunning()) return false;

        Job.Task->Release();
        Job.JobThread->join();

        Jobs.remove(Job);

        return true;
    }
} // Slab::Core