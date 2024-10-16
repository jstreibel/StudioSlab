//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_INTERFACEMANAGER_H
#define STUDIOSLAB_INTERFACEMANAGER_H

#include "Utils/Singleton.h"
#include "Interface.h"

namespace Slab::Core {

    class InterfaceManager : public Singleton<InterfaceManager> {
        UniqueID currentInterface=0;
        UniqueID currentProtocol=0;

        List<Pointer<Interface>> interfaces;

    public:
        InterfaceManager();

        void addInterface(Pointer<Interface>);

        static UniqueID GenerateUniqueID();
    };

} // Slab::Core

#endif //STUDIOSLAB_INTERFACEMANAGER_H
