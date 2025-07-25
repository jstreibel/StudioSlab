//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_INTERFACEMANAGER_H
#define STUDIOSLAB_INTERFACEMANAGER_H

#include "Utils/Singleton.h"
#include "Interface.h"

namespace Slab::Core {

    class FInterfaceManager : public Singleton<FInterfaceManager> {
        UniqueID CurrentInterface=0;
        UniqueID CurrentProtocol=0;

        TList<TPointer<FInterface>> Interfaces;

    public:
        FInterfaceManager();

        void AddInterface(TPointer<FInterface>);

        static UniqueID GenerateUniqueID();
    };

} // Slab::Core

#endif //STUDIOSLAB_INTERFACEMANAGER_H
