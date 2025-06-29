//
// Created by joao on 10/13/24.
//

#ifndef STUDIOSLAB_STUDIOSLAB_H
#define STUDIOSLAB_STUDIOSLAB_H

#include "Core/Backend/BackendManager.h"
#include "Application.h"

namespace Slab {
    class FApplication;

    void Startup();
    void Finish();
    bool IsStarted();

    int Run(FApplication&);

    template<typename AppType>
    int Run(const int argc, const char **argv) {
        AppType app(argc, argv);

        return Run(app);
    }

    Pointer<Core::FBackend> CreatePlatform(Core::FBackendIdentifier);

    template<typename ModuleType = Core::SlabModule>
    ModuleType &GetModule(const Core::FModuleIdentifier &name){
        return dynamic_cast<ModuleType&>(*Core::BackendManager::GetModule(name));
    }
}

#endif //STUDIOSLAB_STUDIOSLAB_H
