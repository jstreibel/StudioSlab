//
// Created by joao on 6/10/24.
//

#include "Task.h"

#include <utility>

namespace Slab::Core {

    Task::Task(Str name) : m_name(std::move(name)), m_continueFlag(false) {
    }

    void Task::start() {
        m_taskStatus = TaskRunning;
        m_threadStatus = ThreadRunning;

        m_taskStatus = run();

        std::unique_lock lock(m_finishMutex);
        m_threadStatus = ThreadWaitingRelease;
        m_releaseCondition.wait(lock, [this] { return m_continueFlag; });
        m_threadStatus = ThreadFinished;
    }

    bool Task::isTaskRunning() const { return m_taskStatus==TaskRunning; }

    Str Task::getName() const { return m_name; }

    TaskStatus Task::getStatus() const {
        return m_taskStatus;
    }

    void Task::release() {
        {
            std::lock_guard lock(m_finishMutex);
            m_continueFlag = true;
        }
        m_releaseCondition.notify_all();
    }
}
