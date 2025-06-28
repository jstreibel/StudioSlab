//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_BACKENDMANAGER_H
#define STUDIOSLAB_BACKENDMANAGER_H

#include "Modules/Modules.h"

#include <memory>

#include "Backend.h"
#include "Implementations.h"
#include "Utils/Map.h"

namespace Slab::Core {

    // Singleton manager for backends
    class BackendManager {
        static FBackendIdentifier BackendName;
        static std::shared_ptr<FBackend> instance;
        static std::map<FModuleIdentifier, std::shared_ptr<SlabModule>> LoadedModules;

        static Map<FBackendIdentifier, FBackendAllocator> AvailableBackends;
        static Map<FModuleIdentifier, FModuleAllocator> AvailableModules;

        static FModuleIdentifier ParseName(const FModuleIdentifier&);
        static void LoadModule(const FModuleIdentifier&);

    public:
        static Pointer<FBackend> GetBackend();

        static void Startup(const Str& BackendName);

        static void RegisterAvailableBackend(const FBackendIdentifier &Name, FBackendAllocator Alloc);;

        static void RegisterAvailableModule(const FModuleIdentifier &Name, FModuleAllocator Alloc);

        template<class ModuleClass>
        static void RegisterAvailableModule(const FModuleIdentifier &Name) {
            RegisterAvailableModule(Name, [](){ return new ModuleClass(); });
        };

        static Pointer<SlabModule> GetModule(const FModuleIdentifier&);
        static bool IsModuleAvailable(const FModuleIdentifier&);
        static bool IsModuleLoaded(const FModuleIdentifier&);

        static Str GetBackendName();

        static void UnloadAllModules();
    };

} // Core

#endif //STUDIOSLAB_BACKENDMANAGER_H
