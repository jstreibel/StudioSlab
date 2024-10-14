//
// Created by joao on 11/05/24.
//
#include "StudioSlab.h"

#include "Core/Base.h"
#include "Graphics/SlabGraphics.h"
#include "Math/SlabMath.h"


namespace Slab {

    void Startup() {
        Core::Register();
        Math::Register();
        Graphics::Register();
    }


}

