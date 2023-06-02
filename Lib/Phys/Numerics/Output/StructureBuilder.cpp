//
// Created by joao on 10/8/21.
//

// #include <RtoR/View/OutputSnapshots.h>
#include "Phys/Numerics/Output/Plugs/OutputConsoleMonitor.h"
#include "StructureBuilder.h"

#include "OutputManager.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"


Numerics::OutputSystem::StructureBuilder::StructureBuilder(String generalDescription)
    : Interface(generalDescription, true)
{
    addParameters({noHistoryToFile, outputResolution,
                   OpenGLMonitor, OpenGLMonitor_startPaused,OpenGLMonitor_stepsPerIdleCall
                   /*&takeSnapshot, &snapshotTime, */ });
}


void Numerics::OutputSystem::StructureBuilder::addConsoleMonitor(OutputManager &outputManager, int nSteps) {
    Plug *out = new OutputConsoleMonitor(nSteps, true);
    outputManager.addOutputChannel(out);
}

