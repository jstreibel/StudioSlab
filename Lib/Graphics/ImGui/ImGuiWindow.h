//
// Created by joao on 6/13/24.
//

#ifndef STUDIOSLAB_IMGUIWINDOW_H
#define STUDIOSLAB_IMGUIWINDOW_H

#include "Utils/Threads.h"
#include "Graphics/Window/SlabWindow.h"


namespace Slab::Graphics {

    class ImGuiWindow : public SlabWindow {
        static Atomic<Count> count;
        Str id;

        Pointer<SlabWindow> slab_window;

    public:
        explicit ImGuiWindow(Pointer<SlabWindow>);

        void draw() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUIWINDOW_H
