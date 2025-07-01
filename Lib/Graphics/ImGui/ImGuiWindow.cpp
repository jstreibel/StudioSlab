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
#include "Graphics/OpenGL/GLStateGuard.h"

#define MyName

namespace Slab::Graphics {
    Atomic<CountType> FImGuiWindow::Count = 0;

    FImGuiWindow::FImGuiWindow(Pointer<FSlabWindow> SlabWindow, Pointer<FImGuiContext> Context)
    : FSlabWindow(SlabWindow->GetConfig())
    , Id(Str("ImGuiWindow##") + ToStr(++Count))
    , SlabWindow(std::move(SlabWindow))
    , Context(std::move(Context))
    {
        if (this->Context == nullptr)
        {
            auto GUIContext = GetGraphicsBackend()->GetMainSystemWindow()->GetGUIContext();
            this->Context = DynamicPointerCast<FImGuiContext>(GUIContext);
        }

        // this->SlabWindow->SetClear(false);
        // this->SlabWindow->SetDecorate(false);
    }

    void FImGuiWindow::ImmediateDraw(const FPlatformWindow& PlatformWindow) {
        // This should be called in case of a local context, which is not the case
        // Context->NewFrame();

        // Context->Render();

        // FSlabWindow::Draw();
    }

    void FImGuiWindow::RegisterDeferredDrawCalls(const FPlatformWindow& PlatformWindow)
    {
        FSlabWindow::RegisterDeferredDrawCalls(PlatformWindow);

        SlabWindow->RegisterDeferredDrawCalls(PlatformWindow);

        CallBackData.SlabWindow = SlabWindow.get();
        CallBackData.PlatformWindow = &PlatformWindow;

        Context->AddDrawCall([this]() {
            if (SlabWindow == nullptr) return;

            ImGui::SetNextWindowSizeConstraints({500,500}, {FLT_MAX, FLT_MAX});
            if(Fix WindowFlags = ImGuiWindowFlags_NoCollapse;
               ImGui::Begin((Str("ImGui-wrapped SlabWindow") + Id).c_str(), nullptr, WindowFlags))
            {
                fix Pos = ImGui::GetWindowPos();
                // fix Dim = ImGui::GetWindowSize();
                fix ContentMin = ImGui::GetWindowContentRegionMin();
                fix ContentMax = ImGui::GetWindowContentRegionMax();

                // auto Backend = GetGraphicsBackend();

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

                // Further defer
                auto Callback = [](const ImDrawList *ParentList, const ImDrawCmd *DrawCommand)
                {
                    OpenGL::FGLStateGuard StateGuard{};

                    if (DrawCommand->UserCallback == nullptr) return;

                    const auto& [SlabWindow, PlatformWindow] = *static_cast<FCallbackData*>(DrawCommand->UserCallbackData);

                    glPushAttrib(GL_VIEWPORT_BIT);
                    SlabWindow->ImmediateDraw(*PlatformWindow);
                    glPopAttrib();
                };

                ImGui::GetWindowDrawList()->AddCallback(Callback, &CallBackData);
            }

            ImGui::End();
        });
    }

} // Slab::Graphics