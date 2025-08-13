//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_PROGRAM_H
#define V_SHAPE_PROGRAM_H

#include <Utils/Types.h>

#include <Utils/Threads.h>

namespace Slab::Core {

    enum ETaskStatus {
        TaskNotInitialized,
        TaskRunning,
        TaskSuccess,
        TaskError,
        TaskAborted
    };

    enum EThreadStatus {
        ThreadNotInitialized,
        ThreadRunning,
        ThreadWaitingRelease,
        ThreadFinished
    };

    class FTask {
    public:

        FTask() = delete;
        explicit FTask(Str name);
        virtual ~FTask() = default;

        /**
         * The entirety of the thread work starts and ends here.
         */
        void start();

        [[nodiscard]] bool IsTaskRunning() const;
        [[nodiscard]] bool IsThreadRunning() const;
        [[nodiscard]] Str GetName() const;
        [[nodiscard]] ETaskStatus GetStatus() const;

        virtual void Abort() = 0;

        /**
         * When the Task finishes running, it waits until it is released to finish.s
         */
        void Release();

    protected:
        virtual ETaskStatus Run() = 0;

    private:
        Str                   Name;
        Atomic<ETaskStatus>   TaskStatus = TaskNotInitialized;
        Atomic<EThreadStatus> ThreadStatus = ThreadNotInitialized;
        Mutex                 FinishMutex;
        Condition             ReleaseCondition;
        bool                  bContinueFlag = false;
    };

    DefinePointers(FTask)

}

#endif //V_SHAPE_PROGRAM_H
