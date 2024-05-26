//
// Created by joao on 09/06/23.
//

#include "InterfaceOwner.h"

#include "Core/Controller/Interface/InterfaceManager.h"
#include "Core/Tools/Log.h"

#include <iomanip>


namespace Slab::Core {

    InterfaceOwner::InterfaceOwner(bool IKnowIMustCallLateStart) {
        if (!IKnowIMustCallLateStart) Log::WarningImportant("Remember to call LateStart!") << Log::Flush;
    }

    InterfaceOwner::InterfaceOwner(Str interfaceName, int priority, bool doRegister) {
        LateStart(interfaceName, priority, doRegister);
    }

    void InterfaceOwner::LateStart(Str interfaceName, int priority, bool doRegister) {
        interface = Slab::New<Interface>(interfaceName, this, priority);

        if (doRegister) registerToManager();
        else
            Log::Debug() << Common::getClassName(this) << ": interface \"" << interface->getName()
                         << "\" did NOT immediately register to InterfaceManager." << Log::Flush;
    }

    void InterfaceOwner::notifyCLArgsSetupFinished() {
        Log::Status() << "Interface " << Log::FGCyan << Log::BoldFace << interface->getName() << Log::ResetFormatting
                      << " (priority " << interface->priority << ") "
                      << "has been setup from command-line." << Log::Flush;

        auto &info = Log::Info() << "Interface " << Log::FGCyan << Log::BoldFace
                                 << interface->getName() << Log::ResetFormatting << " initialized with the following "
                                                                                    "values from command line:";

        for (auto &param: interface->getParameters())
            info << "\n\t\t\t\t\t\t--" << std::left << std::setw(20) << param->getFullCLName() << ": "
                 << param->valueToString();

        info << Log::Flush;
    }

    auto InterfaceOwner::registerToManager() const -> void {
        assert(interface != nullptr);

        InterfaceManager::getInstance().registerInterface(interface);
    }

    auto InterfaceOwner::getInterface() -> Interface_ptr {
        return interface;
    }

    auto InterfaceOwner::getInterface() const -> Interface_ptr {
        return interface;
    }


}