//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_PROGRAM_H
#define V_SHAPE_PROGRAM_H

#include <Utils/Types.h>

#include <atomic>

namespace Slab::Core {

    class Task {
    public:
        enum TaskStatus {
            NotInitialized,
            Running,
            Success,
            Aborted
        };

    protected:
        Str name;
        std::atomic<bool> running = false;
        TaskStatus status = NotInitialized;

        virtual bool run() = 0;

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

    DefinePointer(Task)

}

#endif //V_SHAPE_PROGRAM_H
