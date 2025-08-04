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
        Interface = Slab::New<CLInterface>(interfaceName, this, priority);

        if (doRegister) registerToManager();
        else
            Log::Debug() << Common::getClassName(this) << ": interface \"" << Interface->getName()
                         << "\" did NOT immediately register to InterfaceManager." << Log::Flush;
    }

    void CLInterfaceOwner::NotifyCLArgsSetupFinished() {
        // Log::Info() << "Interface " << Log::FGCyan << Log::BoldFace << interface->getName() << Log::ResetFormatting
        //               << " (priority " << interface->priority << ") "
        //               << "has been setup from command-line." << Log::Flush;

        auto &note = Log::Note() << "Interface " << Log::FGCyan << Log::BoldFace
                                 << Interface->getName() << Log::ResetFormatting
                                 << " (priority " << Interface->priority
                                 << ") initialized with the following values from command line:";

        for (auto &param: Interface->getParameters())
            note << "\n\t\t\t\t\t\t--" << std::left << std::setw(20) << param->getFullCommandLineName() << ": "
                 << param->valueToString();

        note << Log::Flush;
    }

    auto CLInterfaceOwner::registerToManager() const -> void {
        assert(Interface != nullptr);

        Core::RegisterCLInterface(Interface);
    }

    auto CLInterfaceOwner::getInterface() -> CLInterface_ptr {
        return Interface;
    }

    auto CLInterfaceOwner::getInterface() const -> CLInterface_ptr {
        return Interface;
    }

    Vector<Request> CLInterfaceOwner::getProtocols() {
        return Slab::Vector<Request>();
    }


}