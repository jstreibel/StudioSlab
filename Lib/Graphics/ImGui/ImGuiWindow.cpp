//
// Created by joao on 6/13/24.
//

#include "3rdParty/ImGui.h"

#include "ImGuiWindow.h"

#include "Core/Backend/BackendManager.h"

#include <utility>

#define MyName

namespace Slab::Graphics {
    Atomic<Count> ImGuiWindow::count = 0;

    ImGuiWindow::ImGuiWindow(Pointer<Window> SlabWindow)
    : id(Str("ImGuiWindow##") + ToStr(++count))
    , SlabWindow(std::move(SlabWindow)) {

    }



    void ImGuiWindow::draw() {
        if(ImGui::Begin(id.c_str())) {
            if (SlabWindow != nullptr) {
                fix pos = ImGui::GetWindowPos();
                fix dim = ImGui::GetWindowSize();
                fix cMin = ImGui::GetWindowContentRegionMin();
                fix cMax = ImGui::GetWindowContentRegionMax();

                auto &backend = Slab::Core::BackendManager::GetGUIBackend();

                fix x = pos.x + cMin.x;
                fix y = pos.y;
                fix w = cMax.x - cMin.x;
                fix h = cMax.y - cMin.y;

                SlabWindow->setx(x);
                SlabWindow->sety(backend.getScreenHeight() - (y + dim.y));
                SlabWindow->notifyReshape((int) w, (int) h);

                auto callback = [](const ImDrawList *parent_list, const ImDrawCmd *cmd) {
                    if (cmd->UserCallback == NULL) {
                        return;
                    }

                    auto slabWindow = *static_cast<Pointer<Window>*>(cmd->UserCallbackData);
                    slabWindow->draw();
                };
                ImGui::GetWindowDrawList()->AddCallback(callback, &SlabWindow);
            }

            ImGui::End();
        }

        Window::draw();
    }


} // Slab::Graphics