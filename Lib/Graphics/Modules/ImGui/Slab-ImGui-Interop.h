//
// Created by joao on 11/8/24.
//

#ifndef STUDIOSLAB_SLAB_IMGUI_INTEROP_H
#define STUDIOSLAB_SLAB_IMGUI_INTEROP_H

#include "3rdParty/ImGui.h"

#include "Graphics/Backend/Events/KeyMap.h"

namespace Slab::Graphics {
    ImGuiKey SlabToImGuiTranslate(EKeyMap);
}

#endif //STUDIOSLAB_SLAB_IMGUI_INTEROP_H
