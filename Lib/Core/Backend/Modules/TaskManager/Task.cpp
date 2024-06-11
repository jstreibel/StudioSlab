//
// Created by joao on 6/10/24.
//

#include "Task.h"

#include <utility>

namespace Slab::Core {

    Task::Task(Str name) : name(std::move(name)) { }

    void Task::start() {
        running = true;
        status = Running;
        bool good = run();
        running = false;
        if(good) status = Success;
        else status = Aborted;
    }

    bool Task::isRunning() const { return running; }

    Str Task::getName() const { return name; }

    Task::TaskStatus Task::getStatus() const {
        return status;
    }

}
