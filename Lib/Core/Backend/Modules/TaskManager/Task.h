//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_PROGRAM_H
#define V_SHAPE_PROGRAM_H

#include <Utils/Types.h>

#include <Utils/Threads.h>

namespace Slab::Core {

    enum TaskStatus {
        TaskNotInitialized,
        TaskRunning,
        TaskSuccess,
        TaskError,
        TaskAborted
    };

    enum ThreadStatus {
        ThreadNotInitialized,
        ThreadRunning,
        ThreadWaitingRelease,
        ThreadFinished
    };

    class Task {
        Str                  m_name;
        Atomic<TaskStatus>   m_taskStatus = TaskNotInitialized;
        Atomic<ThreadStatus> m_threadStatus = ThreadNotInitialized;
        Mutex                m_finishMutex;
        Condition            m_releaseCondition;
        bool                 m_continueFlag = false;

    protected:
        virtual TaskStatus run() = 0;

    public:

        Task() = delete;
        explicit Task(Str name);
        virtual ~Task() = default;

        /**
         * The entirety of the thread work starts and ends here.
         */
        void start();

        [[nodiscard]] bool isTaskRunning() const;
        [[nodiscard]] bool isThreadRunning() const;
        [[nodiscard]] Str getName() const;
        [[nodiscard]] TaskStatus getStatus() const;

        virtual void abort() = 0;

        /**
         * When the Task finishes running, it waits until it is released to finish.s
         */
        void release();
    };

    DefinePointers(Task)

}

#endif //V_SHAPE_PROGRAM_H
