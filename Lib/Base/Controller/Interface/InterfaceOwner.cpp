//
// Created by joao on 09/06/23.
//

#include "InterfaceOwner.h"

#include "Base/Controller/Interface/InterfaceManager.h"
#include "Base/Tools/Log.h"

InterfaceOwner::InterfaceOwner(bool IKnowIMustCallLateStart) {
    if(!IKnowIMustCallLateStart) Log::WarningImportant("Remember to call LateStart!");
}

InterfaceOwner::InterfaceOwner(Str interfaceName, int priority, bool doRegister){
    LateStart(interfaceName, priority, doRegister);
}

void InterfaceOwner::LateStart(Str interfaceName, int priority, bool doRegister) {
    interface = Interface::New(interfaceName, this, priority);

    if(doRegister) InterfaceManager::getInstance().registerInterface(interface);
    else Log::Debug() << "InterfaceOwner: interface \"" << interface->getName() << "\" will NOT immediately register to InterfaceManager." << Log::Flush;
}

void InterfaceOwner::notifyCLArgsSetupFinished() {
    auto &info = Log::Info();
    info << "Interface " << Log::FGCyan << Log::BoldFace << interface->getName() << Log::ResetFormatting << " (priority " << interface->priority << ") "
                << "has been setup from command-line with the following values: \n";

    for(auto &param : interface->getParameters())
        info << "\t\t\t\t\t\t--" << std::left << std::setw(20) << param->getCLName(true) << ": " << param->valueToString() << "\n";

    info << Log::Flush;
}

auto InterfaceOwner::getInterface()       -> Interface::Ptr {
    return interface;
}

auto InterfaceOwner::getInterface() const -> Interface::Ptr {
    return interface;
}

