//
// Created by joao on 6/13/24.
//

#ifndef STUDIOSLAB_IMGUIWINDOW_H
#define STUDIOSLAB_IMGUIWINDOW_H

#include <Graphics/Modules/ImGui/ImGuiContext.h>

#include "Utils/Threads.h"
#include "Graphics/Window/SlabWindow.h"


namespace Slab::Graphics {

    struct FImGuiWindowContext
    {
        explicit FImGuiWindowContext(const Pointer<FImGuiContext>& Context) : Context(Context) {}

        Pointer<FImGuiContext> Context;
        Str WindowId;
    };

    class FSlabWindow_ImGuiWrapper final : public FSlabWindow {
        static Atomic<CountType> Count;
        Str Id;

        Pointer<FSlabWindow> SlabWindow;
        Pointer<FImGuiContext> Context;

        /**
         * Thread carefully, o lander
         */
        struct FCallbackData
        {
            FSlabWindow* SlabWindow                 = nullptr;
            const FPlatformWindow* PlatformWindow   = nullptr;
        } CallBackData;

    public:
        /**
         * @param SlabWindow The window to wrap with an ImGui window.
         * @param Context The ImGui context within which to show this window. If nullptr, then the
         * main context from the main window is used.
         */
        explicit FSlabWindow_ImGuiWrapper(Pointer<FSlabWindow> SlabWindow, Pointer<FImGuiContext> Context);

        void ImmediateDraw(const FPlatformWindow&) override;

        auto RegisterDeferredDrawCalls(const FPlatformWindow& PlatformWindow) -> void override;
};

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUIWINDOW_H
