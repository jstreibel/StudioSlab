//
// Created by joao on 6/13/24.
//

#include "3rdParty/ImGui.h"

#include "ImGui-SingleSlabWindow-Wrapper.h"

#include "Graphics/SlabGraphics.h"

#include "Core/SlabCore.h"

#include <utility>
#include <Graphics/Modules/ImGui/ImGuiModule.h>

#include "3rdParty/freetype-gl/vertex-attribute.h"
#include "Graphics/OpenGL/GLStateGuard.h"

#define MyName

namespace Slab::Graphics {
    Atomic<CountType> FSlabWindow_ImGuiWrapper::Count = 0;

    FSlabWindow_ImGuiWrapper::FSlabWindow_ImGuiWrapper(TPointer<FSlabWindow> SlabWindow, TPointer<FImGuiContext> Context)
    : FSlabWindow(SlabWindow->GetConfig())
    , Id(Str("ImGuiWindow##") + ToStr(++Count))
    , SlabWindow(std::move(SlabWindow))
    , Context(std::move(Context))
    {
        if (this->Context == nullptr)
        {
            auto GUIContext = GetGraphicsBackend()->GetMainSystemWindow()->SetupGUIContext();
            this->Context = DynamicPointerCast<FImGuiContext>(GUIContext);
        }

        this->SlabWindow->SetClear(false);
        this->SlabWindow->SetDecorate(false);
    }

    void FSlabWindow_ImGuiWrapper::RegisterDeferredDrawCalls(const FPlatformWindow& PlatformWindow)
    {
        FSlabWindow::RegisterDeferredDrawCalls(PlatformWindow);

        CallBackData.SlabWindow = SlabWindow.get();
        CallBackData.PlatformWindow = &PlatformWindow;

        Context->AddDrawCall([this]() {
            if (SlabWindow == nullptr) return;

            ImGui::SetNextWindowSizeConstraints({500,500}, {FLT_MAX, FLT_MAX});
            auto UniqueId = SlabWindow->GetUniqueName();
            if(Fix WindowFlags = ImGuiWindowFlags_NoCollapse;
               ImGui::Begin(UniqueId.c_str(), nullptr, WindowFlags))
            {

                if constexpr (false)
                {

                }
                else if constexpr (false)
                {
                    // Outer table: 2 columns
                    if (ImGui::BeginTable("OuterTable", 2,
                            ImGuiTableFlags_BordersInner | ImGuiTableFlags_Resizable))
                    {
                        // Setup widths
                        ImGui::TableSetupColumn("Left",  ImGuiTableColumnFlags_WidthFixed,  200);
                        ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthStretch);

                        // First row
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Outer C0,R0");

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("Outer C1,R0");

                        // Second row: nest an inner table in C0
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        if (ImGui::BeginTable("InnerTable", 2, ImGuiTableFlags_Borders))
                        {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0); ImGui::Text("I C0,R0");
                            ImGui::TableSetColumnIndex(1); ImGui::Text("I C1,R0");
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0); ImGui::Text("I C0,R1");
                            ImGui::TableSetColumnIndex(1); ImGui::Text("I C1,R1");
                            ImGui::EndTable();
                        }

                        // You can also put any other widgets in the Right cell:
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Button("Click me");

                        ImGui::EndTable();
                    }
                }
                else
                {
                    fix Pos = ImGui::GetWindowPos();
                    fix ContentMin = ImGui::GetWindowContentRegionMin();
                    fix ContentMax = ImGui::GetWindowContentRegionMax();

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
            }

            ImGui::End();
        });

        SlabWindow->RegisterDeferredDrawCalls(PlatformWindow);
    }

} // Slab::Graphics