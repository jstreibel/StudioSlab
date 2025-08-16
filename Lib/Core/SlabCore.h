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
    TPointer<FBackend> GetBackend();

    void LoadModule(const FModuleIdentifier&);
    TPointer<SlabModule> GetModule(const FModuleIdentifier&);

    template <typename ModuleType>
    TPointer<ModuleType>
    GetModule(const FModuleIdentifier& name) {
        auto module_raw = GetModule(name);

        return DynamicPointerCast<ModuleType>(module_raw);
    }

    void RegisterCLInterface(const TPointer<FInterface>&);
    void ParseCLArgs(int, const char**);
}

#endif //STUDIOSLAB_SLABCORE_H
