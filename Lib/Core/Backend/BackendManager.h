//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_BACKENDMANAGER_H
#define STUDIOSLAB_BACKENDMANAGER_H

#include "Modules/Modules.h"

#include <memory>

#include "Backend.h"
#include "GraphicBackend.h"
#include "Implementations.h"

namespace Slab::Core {

    // Singleton manager for backends
    class BackendManager {
        static BackendImplementation backendImplementation;
        static std::unique_ptr<Backend> instance;
        static std::map<Modules, std::shared_ptr<Module>> loadedModules;

    public:
        static Backend& GetBackend();
        static auto GetGUIBackend() -> GraphicBackend&;

        static void Startup(BackendImplementation);

        static void LoadModule(Modules);
        static Module::Ptr GetModule(Modules);
        static bool IsModuleLoaded(Modules);
    };


} // Core

#endif //STUDIOSLAB_BACKENDMANAGER_H
