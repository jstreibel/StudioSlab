//
// Created by joao on 6/13/24.
//

#include "3rdParty/ImGui.h"

#include "ImGuiWindow.h"

#include "Graphics/SlabGraphics.h"

#include "Core/SlabCore.h"

#include <utility>
#include <Graphics/Modules/ImGui/ImGuiModule.h>

#define MyName

namespace Slab::Graphics {
    Atomic<CountType> ImGuiWindow::count = 0;

    ImGuiWindow::ImGuiWindow(Pointer<SlabWindow> slabwindow, Pointer<SlabImGuiContext> imguicontext)
    : id(Str("ImGuiWindow##") + ToStr(++count))
    , slab_window(std::move(slabwindow))
    , imgui_context(std::move(imguicontext)){
        if (imgui_context == nullptr) {
            auto imgui_module = Core::GetModule<ImGuiModule>("ImGui");

            imgui_context = DynamicPointerCast<SlabImGuiContext>(imgui_module->createContext(slab_window->getConfig().parent_syswin));
        }
    }

    void ImGuiWindow::draw() {
        imgui_context->NewFrame();

        imgui_context->AddDrawCall([this]() {
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
                    if (cmd->UserCallback == nullptr) {
                        return;
                    }

                    auto slabWindow = *static_cast<Pointer<SlabWindow>*>(cmd->UserCallbackData);
                    slabWindow->draw();
                };
                ImGui::GetWindowDrawList()->AddCallback(callback, &slab_window);
            }

            ImGui::End();
        }
        });

        imgui_context->Render();

        SlabWindow::draw();
    }


} // Slab::Graphics