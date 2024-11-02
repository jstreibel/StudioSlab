//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_BACKENDMANAGER_H
#define STUDIOSLAB_BACKENDMANAGER_H

#include "Modules/Modules.h"

#include <memory>
#include <utility>

#include "Backend.h"
#include "Implementations.h"
#include "Utils/Map.h"

namespace Slab::Core {

    // Singleton manager for backends
    class BackendManager {
        static BackendName backend_name;
        static std::shared_ptr<Backend> instance;
        static std::map<ModuleName, std::shared_ptr<Module>> loadedModules;

        static Map<BackendName, BackendAllocator> availableBackends;
        static Map<ModuleName, ModuleAllocator> availableModules;

    public:
        static Pointer<Backend> GetBackend();

        static void Startup(const Str& backend_name);


        static void RegisterAvailableBackend(const BackendName &name, BackendAllocator alloc);;

        static void RegisterAvailableModule(const ModuleName &name, ModuleAllocator alloc);

        template<class ModuleClass>
        static void RegisterAvailableModule(const ModuleName &name) {
            RegisterAvailableModule(name, [](){ return new ModuleClass(); });
        };

        static void LoadModule(const ModuleName&);
        static Module::Ptr GetModule(const ModuleName&);
        static bool IsModuleLoaded(const ModuleName&);

        static Str GetBackendName();

        static void UnloadAllModules();
    };

} // Core

#endif //STUDIOSLAB_BACKENDMANAGER_H
