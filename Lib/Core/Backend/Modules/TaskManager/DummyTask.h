//
// Created by joao on 10/30/21.
//

#ifndef STUDIOSLAB_DUMMYPROGRAM_H
#define STUDIOSLAB_DUMMYPROGRAM_H

#include "Task.h"

namespace Slab::Core {

    class FDummyTask : public FTask {
    public:
        FDummyTask() : FTask("Dummy") { }

        auto Run() -> bool override;
    };

    inline auto FDummyTask::Run() -> bool
    {
        return false;
    }
}

#endif //STUDIOSLAB_DUMMYPROGRAM_H
