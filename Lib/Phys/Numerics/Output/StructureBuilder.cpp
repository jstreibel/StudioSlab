//
// Created by joao on 10/8/21.
//

// #include <RtoR/View/OutputSnapshots.h>
#include "Phys/Numerics/Output/Plugs/OutputConsoleMonitor.h"
#include "Builder.h"

#include "OutputManager.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"


Numerics::OutputSystem::Builder::Builder(String generalDescription)
    : InterfaceOwner(generalDescription, 100, false)
{
    interface->addParameters({noHistoryToFile, outputResolution,
                              VisualMonitor, VisualMonitor_startPaused, OpenGLMonitor_stepsPerIdleCall
                              /*&takeSnapshot, &snapshotTime, */ });
}


void Numerics::OutputSystem::Builder::addConsoleMonitor(OutputManager &outputManager, int nSteps) {
    Plug *out = new OutputConsoleMonitor(nSteps);
    outputManager.addOutputChannel(out);
}

