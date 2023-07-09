//
// Created by joao on 10/8/21.
//

// #include <RtoR/View/OutputSnapshots.h>
#include "Phys/Numerics/Output/Plugs/OutputConsoleMonitor.h"
#include "Builder.h"

#include "OutputManager.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"


Numerics::OutputSystem::Builder::Builder(Str generalDescription)
    : InterfaceOwner(generalDescription, 100, false)
{
    interface->addParameters({noHistoryToFile, outputResolution,
                              VisualMonitor, VisualMonitor_startPaused, OpenGLMonitor_stepsPerIdleCall
                              /*&takeSnapshot, &snapshotTime, */ });
}


void Numerics::OutputSystem::Builder::addConsoleMonitor(OutputManager &outputManager, int nSteps) {
    Socket *out = new OutputConsoleMonitor(nSteps);
    outputManager.addOutputChannel(out);
}

