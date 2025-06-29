//
// Created by joao on 6/18/24.
//

#ifndef STUDIOSLAB_PRINTWINDOW_H
#define STUDIOSLAB_PRINTWINDOW_H

#include "Graphics/ImGui/ImGuiWindow.h"

namespace Slab {
    namespace Graphics {

        class PrintWindow : public ImGuiWindow {
        public:
            explicit PrintWindow(const Pointer<FSlabWindow> &slabWindow);
        };

    } // Slab
} // Graphics

#endif //STUDIOSLAB_PRINTWINDOW_H
