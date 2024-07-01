//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_PROGRAM_H
#define V_SHAPE_PROGRAM_H

#include <Utils/Types.h>

#include <Utils/Threads.h>

namespace Slab::Core {

    enum TaskStatus {
        NotInitialized,
        InternalError,
        Running,
        Success,
        Aborted
    };

    class Task {
        Str name;
        Atomic<TaskStatus> taskStatus = NotInitialized;

    protected:
        virtual TaskStatus run() = 0;

    public:

        Task() = delete;
        explicit Task(Str name);
        virtual ~Task() = default;

        void start();

        bool isRunning() const;
        Str getName() const;
        TaskStatus getStatus() const;

        virtual void abort() = 0;
    };

    DefinePointers(Task)

}

#endif //V_SHAPE_PROGRAM_H
