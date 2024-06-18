//
// Created by joao on 6/13/24.
//

#ifndef STUDIOSLAB_IMGUIWINDOW_H
#define STUDIOSLAB_IMGUIWINDOW_H

#include "Utils/Threads.h"
#include "Graphics/Window/Window.h"


namespace Slab::Graphics {

    class ImGuiWindow : public Window {
        static Atomic<Count> count;
        Str id;

        Pointer<Window> SlabWindow;

    public:
        explicit ImGuiWindow(Pointer<Window> SlabWindow);

        void draw() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUIWINDOW_H
