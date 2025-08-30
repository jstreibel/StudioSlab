//
// Created by joao on 09/06/23.
//

#ifndef STUDIOSLAB_CLINTERFACEOWNER_H
#define STUDIOSLAB_CLINTERFACEOWNER_H

#include "Interface.h"


namespace Slab::Core {

    class FInterfaceOwner : public FCommandLineInterfaceListener {
    protected:
        TPointer<FInterface> Interface;

    public:
        explicit FInterfaceOwner(bool IKnowIMustCallLateStart = false);

        virtual
        Vector<FPayload> GetProtocols();

        explicit FInterfaceOwner(Str InterfaceName, int Priority = 10000, bool DoRegister = true);

        void LateStart(Str InterfaceName, int Priority = 10000, bool DoRegister = true);

        auto RegisterToManager() const -> void;

        auto GetInterface() -> TPointer<FInterface>;

        [[nodiscard]] auto GetInterface() const -> TPointer<FInterface>;

        auto SendMessage(FPayload) -> void override;

        virtual auto NotifyInterfaceSetupIsFinished() -> void;

        virtual auto NotifyAllInterfacesSetupIsFinished() -> void ;;
    };


}

#endif //STUDIOSLAB_CLINTERFACEOWNER_H
