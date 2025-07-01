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
        Pointer<FImGuiContext> Context;

    public:
        /**
         * @param SlabWindow The window to wrap with an ImGui window.
         * @param Context The ImGui context within which to show this window. If nullptr, then the
         * main context from the main window is used.
         */
        explicit FImGuiWindow(Pointer<FSlabWindow> SlabWindow, Pointer<FImGuiContext> Context);

        void ImmediateDraw() override;

        auto RegisterDeferredDrawCalls() -> void override;
};

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUIWINDOW_H
