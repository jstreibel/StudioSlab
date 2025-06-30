//
// Created by joao on 6/13/24.
//

#include "3rdParty/ImGui.h"

#include "ImGuiWindow.h"

#include "Graphics/SlabGraphics.h"

#include "Core/SlabCore.h"

#include <utility>
#include <Graphics/Modules/ImGui/ImGuiModule.h>

#include "3rdParty/freetype-gl/vertex-attribute.h"

#define MyName

namespace Slab::Graphics {
    Atomic<CountType> FImGuiWindow::Count = 0;

    FImGuiWindow::FImGuiWindow(Pointer<FSlabWindow> SlabWindow, Pointer<SlabImGuiContext> Context)
    : Id(Str("ImGuiWindow##") + ToStr(++Count))
    , SlabWindow(std::move(SlabWindow))
    , Context(std::move(Context))
    {
        assert(this->Context != nullptr && "Context is null");

        // this->SlabWindow->SetClear(false);
        // this->SlabWindow->SetDecorate(false);
    }

    void FImGuiWindow::ImmediateDraw() {
        // This should be called in case of a local context, which is not the case
        // Context->NewFrame();

        // Context->Render();

        // FSlabWindow::Draw();
    }

    void FImGuiWindow::RegisterDeferredDrawCalls()
    {
        FSlabWindow::RegisterDeferredDrawCalls();

        Context->AddDrawCall([this]() {
            if (SlabWindow == nullptr) return;

            ImGui::SetNextWindowSizeConstraints({500,500}, {FLT_MAX, FLT_MAX});
            fix WindowFlags = ImGuiWindowFlags_NoCollapse;
            if(ImGui::Begin((Str("ImGui-wrapped SlabWindow") + Id).c_str(), nullptr, WindowFlags)) {
                fix Pos = ImGui::GetWindowPos();
                fix Dim = ImGui::GetWindowSize();
                fix ContentMin = ImGui::GetWindowContentRegionMin();
                fix ContentMax = ImGui::GetWindowContentRegionMax();

                auto Backend = GetGraphicsBackend();

                fix w = ContentMax.x - ContentMin.x;
                fix h = ContentMax.y - ContentMin.y;
                fix x = Pos.x + ContentMin.x;
                fix y = Pos.y + ContentMin.y;
                // constexpr auto Min = 400;
                // fix w = dim.x > Min ? dim.x : Min;
                // fix h = dim.y > Min ? dim.y : Min;

                SlabWindow->Set_x(static_cast<int>(x));
                // slab_window->sety(backend.getScreenHeight() - (y + dim.y));
                SlabWindow->Set_y(static_cast<int>(y));
                SlabWindow->NotifyReshape(static_cast<int>(w), static_cast<int>(h));

                if constexpr (true)
                {
                    // Further defer
                    auto Callback = [](const ImDrawList *ParentList, const ImDrawCmd *DrawCommand)
                    {
                        if (DrawCommand->UserCallback == nullptr) return;

                        const auto SlabWindow = *static_cast<Pointer<FSlabWindow>*>(DrawCommand->UserCallbackData);

                        glPushAttrib(GL_VIEWPORT_BIT);
                        SlabWindow->ImmediateDraw();
                        glPopAttrib();
                    };

                    ImGui::GetWindowDrawList()->AddCallback(Callback, &SlabWindow);
                } else
                {
                    glPushAttrib(GL_VIEWPORT_BIT);
                    SlabWindow->ImmediateDraw();
                    glPopAttrib();
                }
            }

            ImGui::End();
        });

        SlabWindow->RegisterDeferredDrawCalls();
    }
} // Slab::Graphics