//
// Created by joao on 6/13/24.
//

#include "3rdParty/ImGui.h"

#include "ImGuiWindow.h"

#include "Graphics/SlabGraphics.h"

#include <utility>

#define MyName

namespace Slab::Graphics {
    Atomic<Count> ImGuiWindow::count = 0;

    ImGuiWindow::ImGuiWindow(Pointer<SlabWindow> slab_window)
    : id(Str("ImGuiWindow##") + ToStr(++count))
    , slab_window(std::move(slab_window)) {

    }



    void ImGuiWindow::draw() {
        if(ImGui::Begin(id.c_str())) {
            if (slab_window != nullptr) {
                fix pos = ImGui::GetWindowPos();
                fix dim = ImGui::GetWindowSize();
                fix cMin = ImGui::GetWindowContentRegionMin();
                fix cMax = ImGui::GetWindowContentRegionMax();

                auto backend = Slab::Graphics::GetGraphicsBackend();

                fix x = pos.x + cMin.x;
                fix y = pos.y;
                fix w = cMax.x - cMin.x;
                fix h = cMax.y - cMin.y;

                slab_window->setx((int)x);
                // slab_window->sety(backend.getScreenHeight() - (y + dim.y));
                slab_window->sety((int)y);
                slab_window->notifyReshape((int) w, (int) h);

                auto callback = [](const ImDrawList *parent_list, const ImDrawCmd *cmd) {
                    if (cmd->UserCallback == NULL) {
                        return;
                    }

                    auto slabWindow = *static_cast<Pointer<SlabWindow>*>(cmd->UserCallbackData);
                    slabWindow->draw();
                };
                ImGui::GetWindowDrawList()->AddCallback(callback, &slab_window);
            }

            ImGui::End();
        }

        SlabWindow::draw();
    }


} // Slab::Graphics