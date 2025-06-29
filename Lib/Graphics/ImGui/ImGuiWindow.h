//
// Created by joao on 6/13/24.
//

#ifndef STUDIOSLAB_IMGUIWINDOW_H
#define STUDIOSLAB_IMGUIWINDOW_H

#include <Graphics/Modules/ImGui/ImGuiContext.h>

#include "Utils/Threads.h"
#include "Graphics/Window/SlabWindow.h"


namespace Slab::Graphics {

    class

    FImGuiWindow final : public FSlabWindow {
        static Atomic<CountType> Count;
        Str Id;

        Pointer<FSlabWindow> SlabWindow;
        Pointer<SlabImGuiContext> Context;

    public:
        explicit FImGuiWindow(Pointer<FSlabWindow>, Pointer<SlabImGuiContext> Context);

        void Draw() override;


    };

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUIWINDOW_H
