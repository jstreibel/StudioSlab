//
// Created by joao on 09/06/23.
//

#include "InterfaceOwner.h"

#include "InterfaceManager.h"
#include "Core/Tools/Log.h"
#include "Core/SlabCore.h"

#include <iomanip>


namespace Slab::Core {

    FInterfaceOwner::FInterfaceOwner(bool IKnowIMustCallLateStart) {
        if (!IKnowIMustCallLateStart) Log::WarningImportant("Remember to call LateStart!") << Log::Flush;
    }

    FInterfaceOwner::FInterfaceOwner(Str InterfaceName, int priority, bool DoRegister) {
        LateStart(InterfaceName, priority, DoRegister);
    }

    void FInterfaceOwner::LateStart(Str InterfaceName, int priority, bool DoRegister) {
        Interface = Slab::New<FInterface>(InterfaceName, this, priority);

        if (DoRegister) RegisterToManager();
        else
            Log::Debug() << Common::getClassName(this) << ": interface \"" << Interface->GetName()
                         << "\" did NOT immediately register to InterfaceManager." << Log::Flush;
    }

    void FInterfaceOwner::NotifyCLArgsSetupFinished() {
        // Log::Info() << "Interface " << Log::FGCyan << Log::BoldFace << interface->getName() << Log::ResetFormatting
        //               << " (priority " << interface->priority << ") "
        //               << "has been setup from command-line." << Log::Flush;

        auto &note = Log::Note() << "Interface " << Log::FGCyan << Log::BoldFace
                                 << Interface->GetName() << Log::ResetFormatting
                                 << " (priority " << Interface->Priority
                                 << ") initialized with the following values from command line:";

        for (auto &param: Interface->GetParameters())
            note << "\n\t\t\t\t\t\t--" << std::left << std::setw(20) << param->GetFullCommandLineName() << ": "
                 << param->ValueToString();

        note << Log::Flush;
    }

    auto FInterfaceOwner::NotifyAllCLArgsSetupFinished() -> void
    {}

    auto FInterfaceOwner::RegisterToManager() const -> void {
        assert(Interface != nullptr);

        Core::RegisterCLInterface(Interface);
    }

    auto FInterfaceOwner::GetInterface() -> TPointer<FInterface> {
        return Interface;
    }

    auto FInterfaceOwner::GetInterface() const -> TPointer<FInterface> {
        return Interface;
    }

    void FInterfaceOwner::SendMessage(FPayload Payload)
    {
        FCommandLineInterfaceListener::SendMessage(Payload);

        if (Payload == FPayload::CommandLineParsingFinished)
            NotifyCLArgsSetupFinished();
        else if (Payload == FPayload::AllCommandLineParsingFinished)
            NotifyAllCLArgsSetupFinished();

        // else
        //     Log::WarningImportant( "Interface " << Log::FGCyan << Log::BoldFace << interface->getName() << Log::ResetFormatting
        //                                 << " (priority " << interface->priority << ") "
        //                                 << "received a message that it does not know how to handle."
    }

    Vector<FPayload> FInterfaceOwner::GetProtocols() {
        return Slab::Vector<FPayload>();
    }


}