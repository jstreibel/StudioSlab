//
// Created by joao on 23/05/24.
//

#include "TaskManager.h"
#include "Core/Tools/Log.h"

namespace Slab::Core {

    TaskManagerModule::TaskManagerModule(TaskManagerModule::DestructorPolicy policy)
    : Module("Task Manager"), destructorPolicy(policy) {    }

    TaskManagerModule::~TaskManagerModule() {
        if(destructorPolicy == WaitAll) {
            for(const auto &job : m_jobs) {
                auto thread = job.second;
                if (thread->joinable()) thread->join();

                auto task = job.first;
            }
        }
        else if(destructorPolicy == AbortAll)
            abortAllTasks();
    }

    auto TaskManagerModule::addTask(const Task_ptr& task) -> TaskManagerModule::Job {
        auto funky = [task]() {
            task->start();

            switch (task->getStatus()) {
                case TaskNotInitialized:
                case TaskRunning:
                    // Task should not be running after finished...
                    Log::Error() << "Job \"" << task->getName() << "\" finished with unexpected status." << Log::Flush;
                    break;
                case TaskSuccess:
                    Log::Success() << "Finished job \"" << task->getName() << "\"." << Log::Flush;
                    break;
                case TaskError:
                    Log::Fail() << "Job \"" << task->getName() << "\" failed (internal task error)." << Log::Flush;
                    break;
                case TaskAborted:
                    Log::Warning() << "Job \"" << task->getName() << "\" aborted." << Log::Flush;
                    break;
            }
        };

        auto thread = New<std::thread>(funky);
        Log::Critical() << "Started job \"" << task->getName() << "\" on thread "
                        << "[id " << thread->get_id() << "] [handle " << thread->native_handle() << "] " << Log::Flush;
        auto job = Job(task, thread);

        {
            std::lock_guard lock(m_addJobMutex); // make sure things go smooth on adding tasks
            m_jobs.emplace_back(job);
        }

        return job;
    }

    void TaskManagerModule::abortAllTasks() {
        for(const auto& job : m_jobs)
            Abort(job);
    }

    void TaskManagerModule::Abort(const Job& job) {
        auto &task = job.first;
        auto &thread = job.second;

        if(task->isTaskRunning()) {
            task->abort();
            Log::Info() << "Sent abort signal to task \"" << task->getName() << "\"." << Log::Flush;
        }

        if(thread->joinable()) {
            Log::Status() << "Waiting for task \"" << task->getName() << "\" thread to join...";
            thread->join();
            Log::Success() << "Task \"" << task->getName() << "\" thread has joined main thread.";
        }

    }

    auto TaskManagerModule::hasRunningTasks() const -> bool {
        return std::ranges::any_of(m_jobs, [](const Job &job){ return job.first->isTaskRunning(); });
    }
} // Slab::Core