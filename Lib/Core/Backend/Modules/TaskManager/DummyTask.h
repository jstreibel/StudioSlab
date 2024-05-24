//
// Created by joao on 10/30/21.
//

#ifndef STUDIOSLAB_DUMMYPROGRAM_H
#define STUDIOSLAB_DUMMYPROGRAM_H

#include "Task.h"

namespace Slab::Core {

    class DummyTask : public Task {
    public:
        bool cycle(CycleOptions options) override { return false; }

        bool run() override {
            return false;
        }


    };

}

#endif //STUDIOSLAB_DUMMYPROGRAM_H
