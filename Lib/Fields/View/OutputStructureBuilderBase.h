//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_OUTPUTSTRUCTUREBUILDERBASE_H
#define FIELDS_OUTPUTSTRUCTUREBUILDERBASE_H

#include "Studios/Controller/Interface/Interface.h"
#include "Studios/Controller/Interface/CommonParameters.h"

class OutputManager;

class OutputStructureBuilderBase: public Interface {
    BoolParameter takeSnapshot = {false, "snapshot,s", "Take a snapshot of simulation at the end."};
    DoubleParameter snapshotTime = {-1.0, "snapshotTime,t", "Force snapshot to be taken at some time prior to end (after will result in no output."};

protected:
    //enum TheBackend {
    //    Console=0,
    //    OpenGL=1
    //} theBackend;

    BoolParameter noHistory = {false, "noOut,o", "Don't output history to file."};
    IntegerParameter outputResolution = {512, "outN", "Output resolution of space dimension in history output."};
    BoolParameter useOpenGL = BoolParameter{false, "gl,g", "Should output to OpenGL"};

    explicit OutputStructureBuilderBase(String generalDescription="Simulation output structure");

    // void buildCommon(OutputManager &outputManager);
    static void addConsoleMonitor(OutputManager &outputManager, int nSteps);

    String fileNameBase;

public:
    virtual auto build() -> OutputManager * = 0;
};


#endif //FIELDS_OUTPUTSTRUCTUREBUILDERBASE_H
