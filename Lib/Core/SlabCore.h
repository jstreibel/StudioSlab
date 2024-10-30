//
// Created by joao on 10/11/24.
//

#ifndef STUDIOSLAB_SLABCORE_H
#define STUDIOSLAB_SLABCORE_H

#include "Backend/BackendManager.h"

namespace Slab::Core {
    void Startup();
    void Finish();

    void StartBackend(const BackendName&);
    Backend &GetBackend();

    void LoadModule(const ModuleName&);
    Pointer<Module> GetModule(const ModuleName&);

    void RegisterCLInterface(const Pointer<CLInterface>&);
    void ParseCLArgs(int, const char**);
}

#endif //STUDIOSLAB_SLABCORE_H
