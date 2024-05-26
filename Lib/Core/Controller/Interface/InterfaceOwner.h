//
// Created by joao on 09/06/23.
//

#ifndef STUDIOSLAB_INTERFACEOWNER_H
#define STUDIOSLAB_INTERFACEOWNER_H

#include "Interface.h"


namespace Slab::Core {

    class InterfaceOwner : public InterfaceListener {
    protected:
        Pointer<Interface> interface;

    public:
        InterfaceOwner(bool IKnowIMustCallLateStart = false);

        explicit InterfaceOwner(Str interfaceName, int priotity = 10000, bool doRegister = true);

        void LateStart(Str interfaceName, int priotity = 10000, bool doRegister = true);

        auto registerToManager() const -> void;

        auto getInterface() -> Pointer<Interface>;

        auto getInterface() const -> Pointer<Interface>;

        auto notifyCLArgsSetupFinished() -> void override;
    };


}

#endif //STUDIOSLAB_INTERFACEOWNER_H
