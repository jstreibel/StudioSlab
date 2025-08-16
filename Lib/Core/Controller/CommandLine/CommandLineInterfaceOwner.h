//
// Created by joao on 09/06/23.
//

#ifndef STUDIOSLAB_CLINTERFACEOWNER_H
#define STUDIOSLAB_CLINTERFACEOWNER_H

#include "CommandLineInterface.h"


namespace Slab::Core {

    class FCommandLineInterfaceOwner : public FCommandLineInterfaceListener {
    protected:
        TPointer<FCommandLineInterface> Interface;

    public:
        explicit FCommandLineInterfaceOwner(bool IKnowIMustCallLateStart = false);

        virtual
        Vector<FPayload> GetProtocols();

        explicit FCommandLineInterfaceOwner(Str InterfaceName, int Priority = 10000, bool DoRegister = true);

        void LateStart(Str InterfaceName, int Priority = 10000, bool DoRegister = true);

        auto RegisterToManager() const -> void;

        auto GetInterface() -> TPointer<FCommandLineInterface>;

        auto GetInterface() const -> TPointer<FCommandLineInterface>;

        auto NotifyCLArgsSetupFinished() -> void override;
    };


}

#endif //STUDIOSLAB_CLINTERFACEOWNER_H
