//
// Created by joao on 23/05/24.
//

#include "TaskManager.h"

namespace Slab::Core {
    void TaskManagerModule::addTask(Task_ptr task) {
        std::lock_guard<std::mutex> lock(mtx);
        threads.emplace_back([task = std::move(task)]() {
            task->run();
        });
    }

    TaskManagerModule::~TaskManagerModule() {
        for (std::thread &t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void TaskManagerModule::signalFinishAllTasks() {
        for(auto task : tasks) task->forceStop();
    }
} // Slab::Core