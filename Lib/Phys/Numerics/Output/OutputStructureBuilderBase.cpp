//
// Created by joao on 10/8/21.
//

// #include <RtoR/View/OutputSnapshots.h>
#include "Phys/Numerics/Output/Channel/OutputConsoleMonitor.h"
#include "OutputStructureBuilderBase.h"

#include "OutputManager.h"


OutputStructureBuilderBase::OutputStructureBuilderBase(String generalDescription)
    : Interface(generalDescription, true)
{
    addParameters({noHistoryToFile, outputResolution,
                   OpenGLMonitor, OpenGLMonitor_startPaused,OpenGLMonitor_stepsPerIdleCall
                   /*&takeSnapshot, &snapshotTime, */ });
}


void OutputStructureBuilderBase::addConsoleMonitor(OutputManager &outputManager, int nSteps) {
    OutputChannel *out = new OutputConsoleMonitor(nSteps, true);
    outputManager.addOutputChannel(out);
}

