//
// Created by joao on 09/06/23.
//

#ifndef STUDIOSLAB_INTERFACEOWNER_H
#define STUDIOSLAB_INTERFACEOWNER_H

#include "Interface.h"

class InterfaceOwner : public InterfaceListener {
protected:
    Interface::Ptr interface;

public:
    InterfaceOwner(String interfaceName, int priotity, bool doRegister);

    auto getInterface() -> Interface::Ptr;

    auto notifyCLArgsSetupFinished() -> void override;
};


#endif //STUDIOSLAB_INTERFACEOWNER_H
