//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_BACKENDMANAGER_H
#define STUDIOSLAB_BACKENDMANAGER_H

#include "GraphicBackend.h"

#include "Implementations.h"
#include "Modules/Modules.h"

namespace Core {

    // Singleton manager for backends
    class BackendManager {
        static BackendImplementation backendImplementation;
        static std::unique_ptr<Backend> instance;

        static std::map<Modules, std::shared_ptr<Module>> loadedModules;

    public:
        static Backend& GetBackend();
        static GraphicBackend& GetGUIBackend();

        static BackendImplementation GetImplementation();

        static void LoadModule(Modules);
        static Module::Ptr GetModule(Modules);
        static bool IsModuleLoaded(Modules);

        static void Startup(BackendImplementation backendImplementation);
    };


} // Core

#endif //STUDIOSLAB_BACKENDMANAGER_H
