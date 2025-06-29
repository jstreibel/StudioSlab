//
// Created by joao on 11/05/24.
//
#include "StudioSlab.h"

#include "Core/SlabCore.h"
#include "Graphics/SlabGraphics.h"
#include "Math/SlabMath.h"
#include "CrashPad.h"

namespace Slab {

    bool Started = false;

    void Startup() {
        if(IsStarted()) return;

        Core::Startup();
        Math::Startup();
        Graphics::Startup();

        Started = true;
    }

    void Finish() {
        Core::Finish();
        Math::Finish();
        Graphics::Finish();
    }

    bool IsStarted() {
        return Started;
    }

    Pointer<Core::FBackend> CreatePlatform(Core::FBackendIdentifier platform) {
        Core::BackendManager::Startup(platform);

        return Core::BackendManager::GetBackend();
    }

    int Run(FApplication &Application) {
        return SafetyNet::Jump(Application);
    }


}

