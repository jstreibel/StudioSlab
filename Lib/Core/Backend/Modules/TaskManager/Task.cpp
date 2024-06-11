//
// Created by joao on 6/10/24.
//

#include "Task.h"

#include <utility>

namespace Slab::Core {

    Task::Task(Str name) : name(std::move(name)) { }

    void Task::start() {
        taskStatus = Running;
        taskStatus = run();
    }

    bool Task::isRunning() const { return taskStatus==Running; }

    Str Task::getName() const { return name; }

    TaskStatus Task::getStatus() const {
        return taskStatus;
    }

}
