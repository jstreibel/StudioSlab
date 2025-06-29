//
// Created by joao on 6/28/25.
//

#ifndef IMGUI_WINDOW_MANAGER_H
#define IMGUI_WINDOW_MANAGER_H

#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Graphics/Window/WindowManager.h"

namespace Slab::Graphics {

    class FImGuiWindowManager : public FWindowManager {

    public:
        explicit FImGuiWindowManager(SystemWindow* Parent, Pointer<SlabImGuiContext>);

        void AddSlabWindow(const Pointer<FSlabWindow>&, bool hidden) override;

    private:
        FImGuiWindowManager() = delete;

        Pointer<SlabImGuiContext> Context;
    };

} // Slab::Graphics

#endif //IMGUI_WINDOW_MANAGER_H
