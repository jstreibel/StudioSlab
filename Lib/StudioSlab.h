//
// Created by joao on 10/13/24.
//

#ifndef STUDIOSLAB_STUDIOSLAB_H
#define STUDIOSLAB_STUDIOSLAB_H

#include "Core/Backend/BackendManager.h"
#include "Application.h"

namespace Slab {
    class Application;

    void Startup();
    void Finish();
    bool IsStarted();

    int Run(Application&);

    template<typename AppType>
    int Run(int argc, char **argv) {
        AppType app(argc, argv);

        return Run(app);
    }

    Pointer<Core::Backend> CreatePlatform(Core::BackendName);

    Core::Module &GetModule(Core::ModuleName);
}

#endif //STUDIOSLAB_STUDIOSLAB_H
