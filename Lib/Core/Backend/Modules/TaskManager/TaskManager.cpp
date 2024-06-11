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
            if(task->getStatus() == Task::Success)
                Log::Success() << "Finished job \"" << task->getName() << "\"." << Log::Flush;
            else
                Log::Warning() << "Job \"" << task->getName() << "\" not finished." << Log::Flush;

        };

        auto thread = New<std::thread>(funky);
        auto job = Job(task, thread);

        jobs.emplace_back(job);

        return job;
    }

    TaskManagerModule::~TaskManagerModule() {
        abortAllTasks();
    }

    void TaskManagerModule::abortAllTasks() {
        for(const auto& job : jobs)
            Abort(job);
    }

    void TaskManagerModule::Abort(Job job) {
        auto &task = job.first;
        auto &thread = job.second;


        if(task->isRunning()) {
            Log::Critical() << "Aborting task \"" << task->getName() << "\"." << Log::Flush;
            task->abort();
            if(thread->joinable()) thread->join();
            Log::Warning() << "Task \"" << task->getName() << "\" aborted." << Log::Flush;
        } else if(thread->joinable()) {
            Log::Error() << "Assertion error: joinable thread should be finished for job \"" << task->getName() << "\".";
        }

    }
} // Slab::Core