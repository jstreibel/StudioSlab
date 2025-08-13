//
// Created by joao on 6/28/25.
//

#ifndef IMGUI_WINDOW_MANAGER_H
#define IMGUI_WINDOW_MANAGER_H

#include "ImGui-SingleSlabWindow-Wrapper.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Graphics/Window/WindowManager.h"

namespace Slab::Graphics {

    class FImGuiWindowManager final : public FWindowManager {

    public:
        explicit FImGuiWindowManager(const TPointer<FImGuiContext>& Context=nullptr);

        void AddSlabWindow(const TPointer<FSlabWindow>&, bool Hidden) override;
        bool NotifyRender(const FPlatformWindow&) override;

    private:
        FImGuiWindowManager() = delete;

        Vector<TPointer<FSlabWindow_ImGuiWrapper>> Windows;
        TPointer<FImGuiContext> Context;
    };

} // Slab::Graphics

#endif //IMGUI_WINDOW_MANAGER_H
