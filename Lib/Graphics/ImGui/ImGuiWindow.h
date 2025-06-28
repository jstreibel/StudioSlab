//
// Created by joao on 6/13/24.
//

#ifndef STUDIOSLAB_IMGUIWINDOW_H
#define STUDIOSLAB_IMGUIWINDOW_H

#include <Graphics/Modules/ImGui/ImGuiContext.h>

#include "Utils/Threads.h"
#include "Graphics/Window/SlabWindow.h"


namespace Slab::Graphics {

    class ImGuiWindow : public SlabWindow {
        static Atomic<CountType> count;
        Str id;

        Pointer<SlabWindow> slab_window;
        Pointer<SlabImGuiContext> imgui_context;

    public:
        explicit ImGuiWindow(Pointer<SlabWindow>, Pointer<SlabImGuiContext> imgui_context=nullptr);

        void draw() override;


    };

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUIWINDOW_H
