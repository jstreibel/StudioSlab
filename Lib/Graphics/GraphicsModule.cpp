//
// Created by joao on 10/13/24.
//

#include "SlabGraphicsModule.h"

#include "3rdParty/ImGui.h"

namespace Slab::Graphics {
    SlabGraphicsModule::SlabGraphicsModule() : GraphicsModule("Slab:Graphics") {}

    void SlabGraphicsModule::beginRender() {
        GraphicsModule::beginRender();


    }
} // Slab::Graphics