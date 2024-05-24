//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_PROGRAM_H
#define V_SHAPE_PROGRAM_H

#include <Utils/Types.h>

namespace Slab::Core {

    class Task;

    class TaskListener {
        void notifyTaskStarted(const Task&);
        void notifyTaskFinished(const Task&);
    };

    class Task {
    public:
        virtual ~Task() = default;

        virtual bool run() = 0;

        virtual void forceStop() = 0;
    };

    DefinePointer(Task)

}

#endif //V_SHAPE_PROGRAM_H
