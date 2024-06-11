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
            Vector<Job> jobs;

            std::mutex mtx;

            enum DestructorPolicy {
                AbortAll,
                WaitAll
            } destructorPolicy;

        public:
            explicit TaskManagerModule(DestructorPolicy policy=AbortAll);
            // Destructor to join all threads
            ~TaskManagerModule() override;

            static void Abort(Job job);

            // Add a task to the manager and start it immediately in a new thread
            Job addTask(Task_ptr task);

            void abortAllTasks();


        };

} // Slab::Core

#endif //STUDIOSLAB_TASKMANAGER_H
