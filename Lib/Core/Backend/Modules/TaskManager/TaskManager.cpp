//
// Created by joao on 23/05/24.
//

#include "TaskManager.h"
#include "Core/Tools/Log.h"

namespace Slab::Core {

    MTaskManager::MTaskManager(MTaskManager::EDestructorPolicy Policy)
    : SlabModule("Task Manager"), destructorPolicy(Policy) {    }

    MTaskManager::~MTaskManager() {
        if(destructorPolicy == WaitAll) {
            for(const auto &job : Jobs) {
                auto thread = job.second;
                if (thread->joinable()) thread->join();

                auto task = job.first;
            }
        }
        else if(destructorPolicy == AbortAll)
            AbortAllTasks();
    }

    auto MTaskManager::AddTask(const FTaskPointer& Task) -> MTaskManager::Job {
        auto funky = [Task]() {
            Task->start();

            switch (Task->GetStatus()) {
                case TaskNotInitialized:
                case TaskRunning:
                    // Task should not be running after finished...
                    Log::Error() << "Job \"" << Task->GetName() << "\" finished with unexpected status." << Log::Flush;
                    break;
                case TaskSuccess:
                    Log::Success() << "Finished job \"" << Task->GetName() << "\"." << Log::Flush;
                    break;
                case TaskError:
                    Log::Fail() << "Job \"" << Task->GetName() << "\" failed (internal task error)." << Log::Flush;
                    break;
                case TaskAborted:
                    Log::Warning() << "Job \"" << Task->GetName() << "\" aborted." << Log::Flush;
                    break;
            }
        };

        auto thread = New<std::thread>(funky);
        Log::Critical() << "Started job \"" << Task->GetName() << "\" on thread "
                        << "[id " << thread->get_id() << "] [handle " << thread->native_handle() << "] " << Log::Flush;
        auto job = Job(Task, thread);

        {
            std::lock_guard lock(AddJobMutex); // make sure things go smooth on adding tasks
            Jobs.emplace_back(job);
        }

        return job;
    }

    void MTaskManager::AbortAllTasks() const
    {
        for(const auto& job : Jobs)
            Abort(job);
    }

    void MTaskManager::Abort(const Job& Job) {
        auto &task = Job.first;
        auto &thread = Job.second;

        Job.first->Release();

        if(task->IsTaskRunning()) {
            task->Abort();
            Log::Info() << "Sent abort signal to task \"" << task->GetName() << "\"." << Log::Flush;
        }

        if(thread->joinable()) {
            Log::Status() << "Waiting for task \"" << task->GetName() << "\" thread to join...";
            thread->join();
            Log::Success() << "Task \"" << task->GetName() << "\" thread has joined main thread.";
        }

    }

    auto MTaskManager::HasRunningTasks() const -> bool {
        return std::ranges::any_of(Jobs, [](const Job &job){ return job.first->IsTaskRunning(); });
    }

    void MTaskManager::PruneThreads()
    {
        NOT_IMPLEMENTED_CLASS_METHOD
    }
} // Slab::Core