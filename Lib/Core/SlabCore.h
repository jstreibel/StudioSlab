//
// Created by joao on 10/11/24.
//

#ifndef STUDIOSLAB_SLABCORE_H
#define STUDIOSLAB_SLABCORE_H

#include "Backend/BackendManager.h"

namespace Slab::Core {
    void Startup();
    void Finish();

    void StartBackend(const FBackendIdentifier&);
    Pointer<FBackend> GetBackend();

    void LoadModule(const FModuleIdentifier&);
    Pointer<SlabModule> GetModule(const FModuleIdentifier&);

    template <typename ModuleType>
    Pointer<ModuleType>
    GetModule(const FModuleIdentifier& name) {
        auto module_raw = GetModule(name);

        return DynamicPointerCast<ModuleType>(module_raw);
    }

    void RegisterCLInterface(const Pointer<FCommandLineInterface>&);
    void ParseCLArgs(int, const char**);
}

#endif //STUDIOSLAB_SLABCORE_H
