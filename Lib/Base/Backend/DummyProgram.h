//
// Created by joao on 10/30/21.
//

#ifndef STUDIOSLAB_DUMMYPROGRAM_H
#define STUDIOSLAB_DUMMYPROGRAM_H

#include "Program.h"

class DummyProgram : public Program {
public:
    void step(PosInt nSteps, void *args= nullptr) override {

    }

    void runFullIntegration() override {

    }
};

#endif //STUDIOSLAB_DUMMYPROGRAM_H
