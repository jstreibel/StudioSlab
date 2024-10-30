//
// Created by joao on 09/06/23.
//

#include "CLInterfaceOwner.h"

#include "CLInterfaceManager.h"
#include "Core/Tools/Log.h"
#include "Core/SlabCore.h"

#include <iomanip>


namespace Slab::Core {

    CLInterfaceOwner::CLInterfaceOwner(bool IKnowIMustCallLateStart) {
        if (!IKnowIMustCallLateStart) Log::WarningImportant("Remember to call LateStart!") << Log::Flush;
    }

    CLInterfaceOwner::CLInterfaceOwner(Str interfaceName, int priority, bool doRegister) {
        LateStart(interfaceName, priority, doRegister);
    }

    void CLInterfaceOwner::LateStart(Str interfaceName, int priority, bool doRegister) {
        interface = Slab::New<CLInterface>(interfaceName, this, priority);

        if (doRegister) registerToManager();
        else
            Log::Debug() << Common::getClassName(this) << ": interface \"" << interface->getName()
                         << "\" did NOT immediately register to InterfaceManager." << Log::Flush;
    }

    void CLInterfaceOwner::notifyCLArgsSetupFinished() {
        // Log::Info() << "Interface " << Log::FGCyan << Log::BoldFace << interface->getName() << Log::ResetFormatting
        //               << " (priority " << interface->priority << ") "
        //               << "has been setup from command-line." << Log::Flush;

        auto &info = Log::Info() << "Interface " << Log::FGCyan << Log::BoldFace
                                 << interface->getName() << Log::ResetFormatting
                                 << " (priority " << interface->priority
                                 << ") initialized with the following values from command line:";

        for (auto &param: interface->getParameters())
            info << "\n\t\t\t\t\t\t--" << std::left << std::setw(20) << param->getFullCommandLineName() << ": "
                 << param->valueToString();

        info << Log::Flush;
    }

    auto CLInterfaceOwner::registerToManager() const -> void {
        assert(interface != nullptr);

        Core::RegisterCLInterface(interface);
    }

    auto CLInterfaceOwner::getInterface() -> CLInterface_ptr {
        return interface;
    }

    auto CLInterfaceOwner::getInterface() const -> CLInterface_ptr {
        return interface;
    }

    Vector<Request> CLInterfaceOwner::getProtocols() {
        return Slab::Vector<Request>();
    }


}