//
// Created by joao on 6/13/24.
//

#ifndef STUDIOSLAB_IMGUI_WINDOW_H
#define STUDIOSLAB_IMGUI_WINDOW_H

#include <Graphics/Modules/ImGui/ImGuiContext.h>

#include <utility>

#include "Utils/Threads.h"
#include "Graphics/Window/SlabWindow.h"


namespace Slab::Graphics {

    struct FImGuiWindowContext
    {
        explicit FImGuiWindowContext(const TPointer<FImGuiContext>& Context, Str  WindowId="")
        : Context(Context), WindowId(std::move(WindowId)) { }

        TPointer<FImGuiContext> Context;
        Str WindowId;
    };

    class FSlabWindow_ImGuiWrapper final : public FSlabWindow
    {

    public:

        /**
         * @param SlabWindow The window to wrap with an ImGui window.
         * @param Context The ImGui context within which to show this window. If nullptr, then the
         * main context from the main window is used.
         */
        explicit FSlabWindow_ImGuiWrapper(TPointer<FSlabWindow> SlabWindow, TPointer<FImGuiContext> Context);

        ~FSlabWindow_ImGuiWrapper() override = default;

        auto ImmediateDraw(const FPlatformWindow&) -> void override;
        auto RegisterDeferredDrawCalls(const FPlatformWindow& PlatformWindow) -> void override;


    private:

        static Atomic<CountType> Count;
        Str Id;

        TPointer<FSlabWindow> SlabWindow;
        TPointer<FImGuiContext> Context;

        /**
         * Thread carefully, o lander
         * You're dealing with raw pointers here
         * This is no man's land in 2025
         */
        struct FCallbackData
        {
            FSlabWindow* SlabWindow                 = nullptr;
            const FPlatformWindow* PlatformWindow   = nullptr;
        } CallBackData;
};

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUI_WINDOW_H
