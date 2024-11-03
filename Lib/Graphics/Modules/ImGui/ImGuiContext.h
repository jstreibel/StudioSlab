//
// Created by joao on 11/3/24.
//

#ifndef STUDIOSLAB_IMGUICONTEXT_H
#define STUDIOSLAB_IMGUICONTEXT_H

#include "Graphics/Modules/GUIModule/GUIContext.h"
#include "3rdParty/ImGui.h"

namespace Slab::Graphics {

    class SlabImGuiContext : public GUIContext {
        ImGuiContext *context = nullptr;
    public:
        SlabImGuiContext(ImGuiContext *context);
        ~SlabImGuiContext() = default;

        void NewFrame();
        void EndFrame(bool render=true);
        void Render();
    };

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUICONTEXT_H
