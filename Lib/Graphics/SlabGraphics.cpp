//
// Created by joao on 10/13/24.
//

#include "SlabGraphics.h"

#include "Core/Backend/BackendManager.h"
#include "GraphicsModule.h"

namespace Slab::Graphics {

    void Register() {
        Core::BackendManager::RegisterAvailableModule<SlabGraphicsModule>("Slab:Graphics");
    }
}