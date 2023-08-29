//
// Created by joao on 10/30/21.
//

#ifndef STUDIOSLAB_DUMMYPROGRAM_H
#define STUDIOSLAB_DUMMYPROGRAM_H

#include "Program.h"

class DummyProgram : public Program {
public:
    bool cycle(CycleOptions options) override {
        return false;
    }
};

#endif //STUDIOSLAB_DUMMYPROGRAM_H
