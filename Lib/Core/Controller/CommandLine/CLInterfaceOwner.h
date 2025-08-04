//
// Created by joao on 09/06/23.
//

#ifndef STUDIOSLAB_CLINTERFACEOWNER_H
#define STUDIOSLAB_CLINTERFACEOWNER_H

#include "CLInterface.h"


namespace Slab::Core {

    class CLInterfaceOwner : public CLInterfaceListener {
    protected:
        Pointer<CLInterface> Interface;

    public:
        explicit CLInterfaceOwner(bool IKnowIMustCallLateStart = false);

        virtual
        Vector<Request> getProtocols();

        explicit CLInterfaceOwner(Str interfaceName, int priotity = 10000, bool doRegister = true);

        void LateStart(Str interfaceName, int priotity = 10000, bool doRegister = true);

        auto registerToManager() const -> void;

        auto getInterface() -> Pointer<CLInterface>;

        auto getInterface() const -> Pointer<CLInterface>;

        auto NotifyCLArgsSetupFinished() -> void override;
    };


}

#endif //STUDIOSLAB_CLINTERFACEOWNER_H
