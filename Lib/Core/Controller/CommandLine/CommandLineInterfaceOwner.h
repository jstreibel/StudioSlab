//
// Created by joao on 09/06/23.
//

#ifndef STUDIOSLAB_CLINTERFACEOWNER_H
#define STUDIOSLAB_CLINTERFACEOWNER_H

#include "CommandLineInterface.h"


namespace Slab::Core {

    class FCommandLineInterfaceOwner : public FCommandLineInterfaceListener {
    protected:
        Pointer<FCommandLineInterface> Interface;

    public:
        explicit FCommandLineInterfaceOwner(bool IKnowIMustCallLateStart = false);

        virtual
        Vector<FRequest> GetProtocols();

        explicit FCommandLineInterfaceOwner(Str InterfaceName, int Priority = 10000, bool DoRegister = true);

        void LateStart(Str InterfaceName, int Priority = 10000, bool DoRegister = true);

        auto RegisterToManager() const -> void;

        auto GetInterface() -> Pointer<FCommandLineInterface>;

        auto GetInterface() const -> Pointer<FCommandLineInterface>;

        auto NotifyCLArgsSetupFinished() -> void override;
    };


}

#endif //STUDIOSLAB_CLINTERFACEOWNER_H
