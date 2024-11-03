//
// Created by joao on 11/05/24.
//
#include "StudioSlab.h"

#include "Core/SlabCore.h"
#include "Graphics/SlabGraphics.h"
#include "Math/SlabMath.h"
#include "CrashPad.h"

namespace Slab {

    bool started = false;

    void Startup() {
        if(IsStarted()) return;

        Core::Startup();
        Math::Startup();
        Graphics::Startup();

        started = true;
    }

    void Finish() {
        Core::Finish();
        Math::Finish();
        Graphics::Finish();
    }

    bool IsStarted() {
        return started;
    }

    Pointer<Core::Backend> CreatePlatform(Core::BackendName platform) {
        Core::BackendManager::Startup(platform);

        return Core::BackendManager::GetBackend();
    }

    int Run(Application &application) {
        return SafetyNet::Jump(application);
    }


}

