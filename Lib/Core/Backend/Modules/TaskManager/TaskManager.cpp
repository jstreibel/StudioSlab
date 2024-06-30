//
// Created by joao on 23/05/24.
//

#include "TaskManager.h"
#include "Core/Tools/Log.h"

namespace Slab::Core {

    TaskManagerModule::TaskManagerModule(TaskManagerModule::DestructorPolicy policy)
    : Module("Task Manager"), destructorPolicy(policy) {    }

    TaskManagerModule::Job TaskManagerModule::addTask(Task_ptr task) {
        std::lock_guard<std::mutex> lock(mtx);

        auto funky = [&task]() {
            Log::Critical() << "Started job \"" << task->getName() << "\"." << Log::Flush;
            task->start();

            auto status = task->getStatus();

            switch (status) {
                case NotInitialized:
                case Running:
                    Log::WarningImportant() << "Job \"" << task->getName() << "\" finished with unexpected status." << Log::Flush;
                    break;
                case Success:
                    Log::Success() << "Finished job \"" << task->getName() << "\"." << Log::Flush;
                    break;
                case InternalError:
                    Log::Fail() << "Job \"" << task->getName() << "\" failed due to internal task error." << Log::Flush;
                    break;
                case Aborted:
                    Log::Warning() << "Job \"" << task->getName() << "\" aborted." << Log::Flush;
                    break;
            }
        };

        auto thread = New<std::thread>(funky);
        auto job = Job(task, thread);

        jobs.emplace_back(job);

        return job;
    }

    TaskManagerModule::~TaskManagerModule() {
        if(destructorPolicy == WaitAll) {
            for(const auto &job : jobs) {
                auto thread = job.second;
                if (thread->joinable()) thread->join();
            }
        }
        else if(destructorPolicy == AbortAll)
            abortAllTasks();
    }

    void TaskManagerModule::abortAllTasks() {
        for(const auto& job : jobs)
            Abort(job);
    }

    void TaskManagerModule::Abort(const Job& job) {
        auto &task = job.first;
        auto &thread = job.second;

        if(task->isRunning()) {
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
        return std::any_of(jobs.begin(), jobs.end(), [](const Job &job){ return job.first->isRunning(); });
    }
} // Slab::Core