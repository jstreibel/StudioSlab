//
// Created by joao on 11/05/24.
//
#include "StudioSlab.h"

#include "Core/SlabCore.h"
#include "Graphics/SlabGraphics.h"
#include "Math/SlabMath.h"


namespace Slab {

    bool started = false;

    void Startup() {
        if(IsStarted()) return;

        Core::Register();
        Math::Register();
        Graphics::Register();

        started = true;
    }

    Core::Module& GetModule(Core::ModuleName name) {
        return *Core::BackendManager::GetModule(name);
    }

    void Finish() {
        Core::Finish();
        Math::Finish();
        Graphics::Finish();
    }

    bool IsStarted() {
        return started;
    }


}

