//
// Created by joao on 10/14/24.
//

#include "InterfaceManager.h"

namespace Slab::Core {
    FInterfaceManager::FInterfaceManager() : Singleton<FInterfaceManager>("Interface Manager"){

    }

    void FInterfaceManager::AddInterface(TPointer<FInterface>) {

    }

    UniqueID FInterfaceManager::GenerateUniqueID() {
        return ++GetInstance().CurrentInterface;
    }
} // Slab::Core