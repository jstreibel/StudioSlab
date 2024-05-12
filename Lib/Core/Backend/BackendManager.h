//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_BACKENDMANAGER_H
#define STUDIOSLAB_BACKENDMANAGER_H

#include "Modules/Modules.h"

#include <memory>

#include "Backend.h"

namespace Core {

    // Singleton manager for backends
    class BackendManager {
        static std::unique_ptr<Backend> instance;

        static std::map<Modules, std::shared_ptr<Module>> loadedModules;

    public:
        static Backend& GetBackend();

        static void LoadModule(Modules);
        static Module::Ptr GetModule(Modules);
        static bool IsModuleLoaded(Modules);

        static void StartupHeadless();
        static void StartupFromBackend(std::unique_ptr<Backend> &backend);
    };


} // Core

#endif //STUDIOSLAB_BACKENDMANAGER_H
