//
// Created by joao on 6/13/24.
//

#include "3rdParty/ImGui.h"

#include "ImGui-SingleSlabWindow-Wrapper.h"

#include "Graphics/SlabGraphics.h"

#include "Core/SlabCore.h"

#include <utility>
#include <Graphics/Modules/ImGui/ImGuiModule.h>

#include "imgui_internal.h"
#include "3rdParty/freetype-gl/vertex-attribute.h"
#include "Core/Tools/Log.h"
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
        if(this->Context == nullptr)
        {
            throw Exception("ImGuiContext is null");
        }

        this->SlabWindow->SetClear(false);
        this->SlabWindow->SetDecorate(false);

        AddResponder(this->SlabWindow);
    }


    void FSlabWindow_ImGuiWrapper::ImmediateDraw(const FPlatformWindow& PlatformWindow)
    {
        if constexpr (false) FSlabWindow::ImmediateDraw(PlatformWindow);

        constexpr bool bPopupMenu = false;

        CallBackData.SlabWindow = SlabWindow.get();
        CallBackData.PlatformWindow = &PlatformWindow;

        ImGui::SetNextWindowSizeConstraints({500,500}, {FLT_MAX, FLT_MAX});
        fix UniqueId = SlabWindow->GetUniqueName();
        bool bOpen = !WantsClose();
        Fix WindowFlags = ImGuiWindowFlags_NoCollapse;
        if(ImGui::Begin(UniqueId.c_str(), &bOpen, WindowFlags))
        {
            constexpr auto bBorder = false;

            fix ChildSize = ImGui::GetContentRegionAvail();

            // Self-contained region with clipping/focus/scroll behavior
            ImGui::BeginChild(SlabWindow->GetTitle().c_str(), ChildSize,
                      bBorder,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            IsFocused = ImGui::IsWindowFocused();

            // 3) Create a real item that owns interactions (blocks others)
            ImVec2 p0    = ImGui::GetCursorScreenPos();
            ImVec2 avail = ImGui::GetContentRegionAvail();
            ImGui::InvisibleButton("CanvasHitArea", avail,
                ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight |
                ImGuiButtonFlags_MouseButtonMiddle);

            // States
            Hovered = ImGui::IsItemHovered();
            Active  = ImGui::IsItemActive();

            // 4) Consume mouse wheel here (prevents parent scroll)
            if (Hovered) {
                fix &IO = ImGui::GetIO();

                if (fix Wheel = IO.MouseWheel; Wheel != 0.0f) {
                    ImGui::SetItemUsingMouseWheel(); // tells ImGui we used the wheel

                    // SlabWindow->NotifyMouseWheel(0.0, Wheel);
                }

                constexpr int MAX_MOUSE_BUTTONS = 5;
                auto MouseClicked = IO.MouseClicked;
            }

            // 5) Mouse interactions while active/hovered
            if (Active && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                // PanBy(ImGui::GetMouseDragDelta());
                ImGui::ResetMouseDragDelta();
            }

            if (Hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                if constexpr (bPopupMenu) ImGui::OpenPopup("canvas_ctx");
            }

            // 6) Keep inputs from propagating to the rest of the app/UI
            if (Hovered || Active) {
                ImGui::SetNextFrameWantCaptureMouse(true);
                ImGui::SetNextFrameWantCaptureKeyboard(true);
            }

            // 7) Draw your content (OpenGL uses the same rect)
            // ImDrawList* dl = ImGui::GetWindowDrawList();
            // dl->AddRectFilled(p0, p0 + avail, IM_COL32(24,24,24,255));
            {
                fix Pos = ImGui::GetWindowPos();
                fix ContentMin_Local = ImGui::GetWindowContentRegionMin();
                fix ContentMax_Local = ImGui::GetWindowContentRegionMax();

                fix ContentWidth  = ContentMax_Local.x - ContentMin_Local.x;
                fix ContentHeight = ContentMax_Local.y - ContentMin_Local.y;
                fix x = Pos.x + ContentMin_Local.x;
                fix y = Pos.y + ContentMin_Local.y;


                // constexpr auto Min = 400;
                // fix w = dim.x > Min ? dim.x : Min;
                // fix h = dim.y > Min ? dim.y : Min;

                SlabWindow->Set_x(static_cast<int>(x));
                // slab_window->sety(backend.getScreenHeight() - (y + dim.y));
                SlabWindow->Set_y(static_cast<int>(y));
                SlabWindow->NotifyReshape(static_cast<int>(ContentWidth), static_cast<int>(ContentHeight));

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

            // SlabWindow->RegisterDeferredDrawCalls(PlatformWindow);

            // Optional: context menu
            if constexpr (bPopupMenu) if (ImGui::BeginPopup("canvas_ctx")) {
                // menu items...
                ImGui::EndPopup();
            }

            ImGui::EndChild();
        }

        ImGui::End();

        if (bOpen == false) this->Close();
    }

    bool FSlabWindow_ImGuiWrapper::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys)
    {
        if (!IsFocused) return false;

        return FSlabWindow::NotifyKeyboard(key, state, modKeys);
    }

    bool FSlabWindow_ImGuiWrapper::NotifyCharacter(UInt codepoint)
    {
        if (!IsFocused) return false;

        return FSlabWindow::NotifyCharacter(codepoint);
    }

    bool FSlabWindow_ImGuiWrapper::NotifyMouseButton(EMouseButton mouse_button, EKeyState key_state,
        EModKeys mod_keys)
    {
        if (!IsFocused) return false;

        return FSlabWindow::NotifyMouseButton(mouse_button, key_state, mod_keys);
    }

    bool FSlabWindow_ImGuiWrapper::NotifyMouseMotion(int x, int y, int dx, int dy)
    {
        if (!IsFocused) return false;

        return FSlabWindow::NotifyMouseMotion(x, y, dx, dy);
    }

    bool FSlabWindow_ImGuiWrapper::NotifyMouseWheel(double dx, double dy)
    {
        if (!IsFocused) return false;

        return FSlabWindow::NotifyMouseWheel(dx, dy);
    }
} // Slab::Graphics