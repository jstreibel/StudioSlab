//
// Created by joao on 09/06/23.
//

#include "InterfaceOwner.h"

#include "Base/Controller/Interface/InterfaceManager.h"
#include "Common/Log/Log.h"

InterfaceOwner::InterfaceOwner(bool IKnowIMustCallLateStart) {
    if(!IKnowIMustCallLateStart) Log::WarningImportant("Remember to call LateStart!");
}

InterfaceOwner::InterfaceOwner(String interfaceName, int priority, bool doRegister){
    LateStart(interfaceName, priority, doRegister);
}

void InterfaceOwner::LateStart(String interfaceName, int priority, bool doRegister) {
    interface = Interface::New(interfaceName, this, priority);

    if(doRegister) InterfaceManager::getInstance().registerInterface(interface);
    else Log::Note() << "Interface \"" << interface->getName() << "\" will NOT be immediately registered in InterfaceManager." << Log::Flush;
}

void InterfaceOwner::notifyCLArgsSetupFinished() {
    Log::Note() << "Interface " << Log::ForegroundCyan << Log::BoldFace << interface->getName() << Log::ResetFormatting << " (priority " << interface->priority << ") "
                << "has been setup from command-line with the following values:" << Log::Flush;

    for(auto &param : interface->getParameters())
        Log::Note() << "\t\t--" << std::left << std::setw(20) << param->getCommandLineArgName(true) << ": " << param->valueToString() << Log::Flush;
}

auto InterfaceOwner::getInterface() -> Interface::Ptr {
    return interface;
}


