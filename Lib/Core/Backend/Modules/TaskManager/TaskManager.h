//
// Created by joao on 23/05/24.
//

#ifndef STUDIOSLAB_TASKMANAGER_H
#define STUDIOSLAB_TASKMANAGER_H

#include <thread>
#include "../Module.h"

#include "Task.h"

namespace Slab::Core {

        class TaskManagerModule : public Module {
            typedef Pair<Task_ptr, Pointer<std::thread>> Job;
            Vector<Job> m_jobs;

            std::mutex m_addJobMutex;

            enum DestructorPolicy {
                AbortAll,
                WaitAll
            } destructorPolicy;

        public:
            explicit TaskManagerModule(DestructorPolicy policy=AbortAll);
            // Destructor to join all threads
            ~TaskManagerModule() override;

            [[nodiscard]] auto hasRunningTasks() const -> bool;

            /**
             * Signals all finished jobs to end their threads in a safe manner.
             */
            void pruneThreads();

            static void Abort(const Job& job);

            /**
             * Add a task to the manager and start it immediately in a new thread.
             */
            Job addTask(const Pointer<Task>& task);

            void abortAllTasks();


        };

} // Slab::Core

#endif //STUDIOSLAB_TASKMANAGER_H
