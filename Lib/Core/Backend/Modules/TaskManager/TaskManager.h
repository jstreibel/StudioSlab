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
        private:
            Vector<Task_ptr> tasks;
            Vector<std::thread> threads;
            std::mutex mtx;

        public:
            // Add a task to the manager and start it immediately in a new thread
            void addTask(Task_ptr task);

            void signalFinishAllTasks();

            // Destructor to join all threads
            ~TaskManagerModule() override;
        };

} // Slab::Core

#endif //STUDIOSLAB_TASKMANAGER_H
