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
    Atomic<CountType> ImGuiWindow::Count = 0;

    ImGuiWindow::ImGuiWindow(Pointer<FSlabWindow> SlabWindow, Pointer<SlabImGuiContext> Context)
    : Id(Str("ImGuiWindow##") + ToStr(++Count))
    , SlabWindow(std::move(SlabWindow))
    , Context(std::move(Context))
    {
        if (Context == nullptr) {
            auto imgui_module = Core::GetModule<ImGuiModule>("ImGui");

            Context = DynamicPointerCast<SlabImGuiContext>(imgui_module->createContext(SlabWindow->getConfig().parent_syswin));
        }
    }

    void ImGuiWindow::Draw() {
        Context->NewFrame();

        Context->AddDrawCall([this]() {
            if(ImGui::Begin(Id.c_str())) {
            if (SlabWindow != nullptr) {
                fix pos = ImGui::GetWindowPos();
                fix dim = ImGui::GetWindowSize();
                fix cMin = ImGui::GetWindowContentRegionMin();
                fix cMax = ImGui::GetWindowContentRegionMax();

                auto Backend = GetGraphicsBackend();

                fix x = pos.x + cMin.x;
                fix y = pos.y;
                fix w = cMax.x - cMin.x;
                fix h = cMax.y - cMin.y;

                SlabWindow->Set_x((int)x);
                // slab_window->sety(backend.getScreenHeight() - (y + dim.y));
                SlabWindow->Set_y((int)y);
                SlabWindow->NotifyReshape((int) w, (int) h);

                auto Callback = [](const ImDrawList */*ParentList*/, const ImDrawCmd *DrawCommand)
                {
                    if (DrawCommand->UserCallback == nullptr) return;

                    const auto SlabWindow = *static_cast<Pointer<FSlabWindow>*>(DrawCommand->UserCallbackData);
                    SlabWindow->Draw();
                };
                ImGui::GetWindowDrawList()->AddCallback(Callback, &SlabWindow);
            }

            ImGui::End();
        }
        });

        Context->Render();

        FSlabWindow::Draw();
    }


} // Slab::Graphics