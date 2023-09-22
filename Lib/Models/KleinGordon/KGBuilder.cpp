//
// Created by joao on 20/09/23.
//

#include "KGBuilder.h"

#include "Core/Controller/Interface/InterfaceManager.h"

Fields::KleinGordon::KGBuilder::KGBuilder(const Str& name, Str generalDescription, bool doRegister)
        : Core::Simulation::VoidBuilder(name, std::move(generalDescription), DONT_REGISTER) {

    interface->addParameters({&noHistoryToFile,
                              &outputResolution,
                              &VisualMonitor,
                              &VisualMonitor_startPaused,
                              &OpenGLMonitor_stepsPerIdleCall,
                              &takeSnapshot,
                              &takeDFTSnapshot,
                              /*, &snapshotTime, */ });

    if(doRegister) InterfaceManager::getInstance().registerInterface(interface);
}
