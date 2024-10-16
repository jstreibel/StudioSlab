//
// Created by joao on 10/14/24.
//

#include "InterfaceManager.h"

namespace Slab::Core {
    InterfaceManager::InterfaceManager() : Singleton<InterfaceManager>("Interface Manager"){

    }

    void InterfaceManager::addInterface(Pointer<Interface>) {

    }

    UniqueID InterfaceManager::GenerateUniqueID() {
        return ++GetInstance().currentInterface;
    }
} // Slab::Core