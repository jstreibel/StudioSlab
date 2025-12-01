//
// Created by joao on 10/24/25.
//

#include "App.h"

#include "Core/SlabCore.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Modules/Animator/Animator.h"
#include "Physics/Foils/NACA2412.h"
#include "Plane/FPlaneController.h"
#include "Render/Drawables.h"
#include "Render/PlaneStats.h"
#include "Scenes/BigHill.h"

#include  "Scenes/Scene.h"
#include <imgui.h>

namespace {
    ImVec4 ToVec4(const Slab::Graphics::FColor c) { return ImVec4(c.r, c.g, c.b, c.a); }

    void SetLittlePlaneDesignerTheme() {
        using namespace Slab::Graphics;

        auto &style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(12.0f, 10.0f);
        style.FramePadding = ImVec2(8.0f, 5.0f);
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.WindowRounding = 8.0f;
        style.ChildRounding = 6.0f;
        style.FrameRounding = 6.0f;
        style.GrabRounding = 5.0f;
        style.ScrollbarRounding = 6.0f;
        style.TabRounding = 5.0f;
        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;

        auto &colors = style.Colors;
        const auto bgDeep = ToVec4(BlueprintDark);
        const auto bg = ToVec4(Blueprint);
        const auto bgLight = ToVec4(BlueprintLight);
        const auto accent = ToVec4(GreyBlue);
        const auto stroke = ToVec4(WhiteBlueprintStroke);

        colors[ImGuiCol_Text] = stroke;
        colors[ImGuiCol_TextDisabled] = ImVec4(stroke.x, stroke.y, stroke.z, 0.6f);
        colors[ImGuiCol_WindowBg] = ImVec4(bgDeep.x, bgDeep.y, bgDeep.z, 0.97f);
        colors[ImGuiCol_ChildBg] = ImVec4(bgDeep.x, bgDeep.y, bgDeep.z, 0.9f);
        colors[ImGuiCol_PopupBg] = ImVec4(bg.x, bg.y, bg.z, 0.98f);
        colors[ImGuiCol_Border] = ImVec4(stroke.x, stroke.y, stroke.z, 0.35f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
        colors[ImGuiCol_FrameBg] = ImVec4(bg.x, bg.y, bg.z, 0.8f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 0.9f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 1.0f);
        colors[ImGuiCol_TitleBg] = bgDeep;
        colors[ImGuiCol_TitleBgActive] = ImVec4(bg.x, bg.y, bg.z, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(bgDeep.x, bgDeep.y, bgDeep.z, 0.8f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(bg.x, bg.y, bg.z, 0.95f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(bgDeep.x, bgDeep.y, bgDeep.z, 0.8f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 0.8f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 0.9f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 1.0f);
        colors[ImGuiCol_CheckMark] = stroke;
        colors[ImGuiCol_SliderGrab] = accent;
        colors[ImGuiCol_SliderGrabActive] = ToVec4(White);
        colors[ImGuiCol_Button] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 0.45f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 0.7f);
        colors[ImGuiCol_ButtonActive] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 0.9f);
        colors[ImGuiCol_Header] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 0.55f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 0.75f);
        colors[ImGuiCol_HeaderActive] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 0.95f);
        colors[ImGuiCol_Separator] = ImVec4(stroke.x, stroke.y, stroke.z, 0.35f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(stroke.x, stroke.y, stroke.z, 0.55f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(stroke.x, stroke.y, stroke.z, 0.75f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(accent.x, accent.y, accent.z, 0.35f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(accent.x, accent.y, accent.z, 0.55f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(accent.x, accent.y, accent.z, 0.85f);
        colors[ImGuiCol_Tab] = ImVec4(bg.x, bg.y, bg.z, 0.75f);
        colors[ImGuiCol_TabHovered] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 0.85f);
        colors[ImGuiCol_TabActive] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 0.95f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(bg.x, bg.y, bg.z, 0.6f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(bgLight.x, bgLight.y, bgLight.z, 0.8f);
        colors[ImGuiCol_PlotLines] = ImVec4(stroke.x, stroke.y, stroke.z, 0.9f);
        colors[ImGuiCol_PlotLinesHovered] = accent;
        colors[ImGuiCol_PlotHistogram] = accent;
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(accent.x, accent.y, accent.z, 0.8f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(GrassGreen.r, GrassGreen.g, GrassGreen.b, 0.9f);
        colors[ImGuiCol_NavHighlight] = ImVec4(accent.x, accent.y, accent.z, 0.85f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(stroke.x, stroke.y, stroke.z, 0.4f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(bgDeep.x, bgDeep.y, bgDeep.z, 0.5f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(bgDeep.x, bgDeep.y, bgDeep.z, 0.7f);
    }
}

FLittlePlaneDesignerApp::FLittlePlaneDesignerApp(const int argc, const char* argv[])
: FApplication("Little Plane Designer", argc, argv) { }

bool FLittlePlaneDesignerApp::NotifyRender(const Graphics::FPlatformWindow& PlatformWindow)  {
    using namespace Slab;

    fix &MouseState = *PlatformWindow.GetMouseState();
    fix &KeyboardState = *PlatformWindow.GetKeyboardState();

    PlatformWindow.Clear(Graphics::Black);

    Controller->HandleInputState({KeyboardState, MouseState});

    CurrentScene->Tick(1.f/60.f);
    CurrentScene->Draw({PlatformWindow});

    return true;
}

bool FLittlePlaneDesignerApp::NotifyKeyboard(
    const Graphics::EKeyMap key,
    const Graphics::EKeyState state,
    const Graphics::EModKeys modKeys)
{
    if (state == Graphics::EKeyState::Press)
    {
        if (modKeys.Mod_Alt) {
            if (key == Graphics::EKeyMap::Key_F4) {
                GetPlatform()->GetMainSystemWindow()->SignalClose();
                return true;
            }
        }

        if (key == Graphics::EKeyMap::Key_F5) { SetSceneBigHill(); return true; }
        if (key == Graphics::EKeyMap::Key_F6) { SetSceneBlueprint(); return true; }

        if (key == Graphics::EKeyMap::Key_SPACE) { CurrentScene->TogglePause(); return true; }
    }

    return Controller->NotifyKeyboard(key, state, modKeys);
}


void FLittlePlaneDesignerApp::OnStart() {
    Core::LoadModule("ModernOpenGL");
    Core::GetModule("ImGui");

    const auto SystemWindow = Graphics::GetGraphicsBackend()->GetMainSystemWindow();
    SystemWindow->SetupGUIContext();
    SetLittlePlaneDesignerTheme();

    BlueprintScene = Slab::New<FLittlePlaneBlueprint>();
    auto PlaneFactory = BlueprintScene->GetPlaneFactory();
    BigHill = Slab::New<FBigHill>(PlaneFactory);

    Controller = BigHill;
    CurrentScene = BigHill;

    CurrentScene->Startup(*SystemWindow);
}

void FLittlePlaneDesignerApp::SetSceneBigHill() {
    CurrentScene = BigHill;
    Controller = BigHill;
}

void FLittlePlaneDesignerApp::SetSceneBlueprint() {
    CurrentScene = BlueprintScene;
    Controller = BlueprintScene;
}
