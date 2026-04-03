#include "Graphics/Window/V2/WorkspaceShellV2.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <vector>

namespace {

    namespace WindowingV2 = Slab::Graphics::Windowing::V2;

    constexpr auto WorkspaceIdExplore = "explore";
    constexpr auto WorkspaceIdPlots = "plots";
    constexpr auto WorkspaceIdAssets = "assets";

    constexpr auto ThemeIdStudioSlab = "studio_slab";
    constexpr auto ThemeIdScientificPaper = "scientific_paper";
    constexpr auto ThemeIdScientificSlate = "scientific_slate";
    constexpr auto ThemeIdBlueprintNight = "blueprint_night";

    constexpr auto WindowTitleSceneOutline = "Scene Outline";
    constexpr auto WindowTitleViewportPreview = "Viewport Preview";
    constexpr auto WindowTitleInspector = "Inspector";
    constexpr auto WindowTitleConsole = "Console";

    constexpr auto WindowTitleSeriesControls = "Series Controls";
    constexpr auto WindowTitlePlotSurface = "Plot Surface";
    constexpr auto WindowTitleMetrics = "Metrics";

    constexpr auto WindowTitleAssetBrowser = "Asset Browser";
    constexpr auto WindowTitlePreviewBoard = "Preview Board";
    constexpr auto WindowTitleDetails = "Details";

    struct FWorkspaceSandboxApp {
        GLFWwindow *Window = nullptr;
        ImVec4 ClearColor = ImVec4(0.06f, 0.08f, 0.11f, 1.0f);
        WindowingV2::FWorkspaceShellStateV2 WorkspaceShellState;
        Slab::Str ActiveWorkspaceId = WorkspaceIdExplore;
        Slab::Str ActiveThemeId = ThemeIdStudioSlab;

        bool bShowSceneOutline = true;
        bool bShowViewportPreview = true;
        bool bShowInspector = true;
        bool bShowConsole = true;

        bool bShowSeriesControls = true;
        bool bShowPlotSurface = true;
        bool bShowMetrics = true;

        bool bShowAssetBrowser = true;
        bool bShowPreviewBoard = true;
        bool bShowDetails = true;

        bool bAnimatePreview = true;
        bool bShowGrid = true;
        bool bShowMarkers = true;
        bool bShowImGuiDemo = false;
        float PreviewZoom = 1.0f;
        float PreviewAmplitude = 0.78f;
        float PreviewFrequency = 1.8f;
        int SelectedAssetIndex = 1;
        int SelectedSeriesIndex = 0;
        Slab::Vector<Slab::Str> EventLog = {
            "WASM workspace sample booted",
            "Shared workspace shell is active"
        };
    };

    struct FThemeDescriptor {
        const char *ThemeId = "";
        const char *DisplayName = "";
        const char *LauncherLabel = "";
        ImVec4 ClearColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        void (*ApplyStyle)(ImGuiStyle &) = nullptr;
    };

    auto ErrorCallback(const int errorCode, const char *description) -> void {
        std::fprintf(stderr, "GLFW error %d: %s\n", errorCode, description);
    }

    auto ResetStyleDarkBase() -> ImGuiStyle {
        ImGuiStyle style;
        ImGui::StyleColorsDark(&style);
        return style;
    }

    auto ResetStyleLightBase() -> ImGuiStyle {
        ImGuiStyle style;
        ImGui::StyleColorsLight(&style);
        return style;
    }

    auto ApplyDeterministicMetrics(ImGuiStyle &style, const bool compact = false) -> void {
        const float sizeFactor = compact ? 0.90f : 1.0f;

        style.Alpha = 1.0f;
        style.DisabledAlpha = 0.58f;

        style.WindowPadding = ImVec2(10.0f * sizeFactor, 8.0f * sizeFactor);
        style.WindowRounding = 4.0f;
        style.WindowBorderSize = 1.0f;
        style.WindowMinSize = ImVec2(120.0f, 96.0f);
        style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
        style.WindowMenuButtonPosition = ImGuiDir_Left;

        style.ChildRounding = 4.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupRounding = 4.0f;
        style.PopupBorderSize = 1.0f;

        style.FramePadding = ImVec2(8.0f * sizeFactor, 5.0f * sizeFactor);
        style.FrameRounding = 3.0f;
        style.FrameBorderSize = 1.0f;

        style.ItemSpacing = ImVec2(9.0f * sizeFactor, 6.0f * sizeFactor);
        style.ItemInnerSpacing = ImVec2(7.0f * sizeFactor, 4.0f * sizeFactor);
        style.CellPadding = ImVec2(7.0f * sizeFactor, 4.0f * sizeFactor);

        style.IndentSpacing = 22.0f * sizeFactor;
        style.ScrollbarSize = 15.0f * sizeFactor;
        style.ScrollbarRounding = 4.0f;
        style.GrabMinSize = 11.0f * sizeFactor;
        style.GrabRounding = 3.0f;

        style.TabRounding = 4.0f;
        style.TabBorderSize = 1.0f;
        style.TabBarBorderSize = 1.0f;
        style.TabBarOverlineSize = 2.0f;
        style.SeparatorTextBorderSize = 1.0f;
        style.SeparatorTextAlign = ImVec2(0.0f, 0.5f);
        style.SeparatorTextPadding = ImVec2(20.0f, style.FramePadding.y);
        style.DockingSeparatorSize = 2.0f;
    }

    auto SetScientificSlatePalette(ImVec4 *colors) -> void {
        colors[ImGuiCol_Text] = ImVec4(0.86f, 0.90f, 0.93f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.50f, 0.55f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.11f, 0.14f, 0.98f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.12f, 0.15f, 0.98f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.13f, 0.16f, 0.98f);
        colors[ImGuiCol_Border] = ImVec4(0.23f, 0.28f, 0.33f, 0.95f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.17f, 0.21f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.17f, 0.24f, 0.30f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.29f, 0.36f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.18f, 0.23f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.09f, 0.12f, 0.15f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.13f, 0.20f, 0.27f, 0.75f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.30f, 0.38f, 0.78f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.22f, 0.37f, 0.46f, 0.90f);
        colors[ImGuiCol_Button] = ImVec4(0.14f, 0.22f, 0.30f, 0.72f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.31f, 0.40f, 0.86f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.41f, 0.52f, 0.92f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.37f, 0.84f, 0.89f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.33f, 0.73f, 0.79f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.42f, 0.88f, 0.96f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.16f, 0.21f, 0.92f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.30f, 0.38f, 0.94f);
        colors[ImGuiCol_TabActive] = ImVec4(0.17f, 0.25f, 0.32f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.24f, 0.30f, 0.37f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.73f, 0.80f, 0.35f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.36f, 0.73f, 0.80f, 0.70f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.36f, 0.73f, 0.80f, 0.95f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.46f, 0.60f, 0.35f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.32f, 0.70f, 0.76f, 0.62f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.17f, 0.21f, 0.26f, 0.30f);
    }

    auto SetScientificPaperPalette(ImVec4 *colors) -> void {
        colors[ImGuiCol_Text] = ImVec4(0.17f, 0.15f, 0.13f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.47f, 0.42f, 0.38f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.92f, 0.88f, 0.98f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.92f, 0.90f, 0.86f, 0.95f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.97f, 0.95f, 0.92f, 0.98f);
        colors[ImGuiCol_Border] = ImVec4(0.58f, 0.53f, 0.47f, 0.70f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.89f, 0.86f, 0.81f, 0.85f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.83f, 0.79f, 0.72f, 0.90f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.74f, 0.66f, 0.95f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.82f, 0.78f, 0.71f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.76f, 0.70f, 0.61f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.87f, 0.84f, 0.78f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.72f, 0.67f, 0.58f, 0.65f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.67f, 0.60f, 0.48f, 0.74f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.61f, 0.54f, 0.41f, 0.82f);
        colors[ImGuiCol_Button] = ImVec4(0.69f, 0.62f, 0.50f, 0.58f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.62f, 0.54f, 0.40f, 0.74f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.52f, 0.44f, 0.30f, 0.86f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.31f, 0.52f, 0.65f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.61f, 0.74f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.33f, 0.51f, 0.64f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.82f, 0.78f, 0.70f, 0.85f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.74f, 0.68f, 0.58f, 0.90f);
        colors[ImGuiCol_TabActive] = ImVec4(0.68f, 0.61f, 0.49f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.60f, 0.54f, 0.46f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.48f, 0.62f, 0.72f, 0.38f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.48f, 0.62f, 0.72f, 0.70f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.48f, 0.62f, 0.72f, 0.95f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.48f, 0.62f, 0.72f, 0.28f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.42f, 0.58f, 0.68f, 0.45f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.80f, 0.75f, 0.67f, 0.19f);
    }

    auto SetBlueprintNightPalette(ImVec4 *colors) -> void {
        colors[ImGuiCol_Text] = ImVec4(0.78f, 0.91f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.46f, 0.60f, 0.73f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.08f, 0.15f, 0.98f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.04f, 0.09f, 0.17f, 0.98f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.10f, 0.19f, 0.98f);
        colors[ImGuiCol_Border] = ImVec4(0.17f, 0.38f, 0.58f, 0.72f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.16f, 0.25f, 0.95f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.11f, 0.22f, 0.34f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.14f, 0.28f, 0.41f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.03f, 0.08f, 0.14f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.20f, 0.31f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.05f, 0.11f, 0.18f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.11f, 0.25f, 0.38f, 0.76f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.14f, 0.33f, 0.50f, 0.84f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.17f, 0.41f, 0.60f, 0.92f);
        colors[ImGuiCol_Button] = ImVec4(0.12f, 0.27f, 0.41f, 0.72f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.16f, 0.37f, 0.55f, 0.84f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.45f, 0.66f, 0.94f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.49f, 0.86f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.46f, 0.79f, 0.98f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.90f, 1.00f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.17f, 0.25f, 0.90f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.30f, 0.43f, 0.94f);
        colors[ImGuiCol_TabActive] = ImVec4(0.13f, 0.26f, 0.37f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.39f, 0.55f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.50f, 0.84f, 1.00f, 0.38f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.50f, 0.84f, 1.00f, 0.73f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.84f, 1.00f, 0.97f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.21f, 0.47f, 0.66f, 0.36f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.41f, 0.76f, 0.95f, 0.56f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.10f, 0.19f, 0.28f, 0.28f);
    }

    auto ApplyStyleStudioSlab(ImGuiStyle &style) -> void {
        style = ResetStyleDarkBase();
        ApplyDeterministicMetrics(style);
        auto *colors = style.Colors;

        colors[ImGuiCol_Text] = ImVec4(0.83f, 0.86f, 0.88f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.48f, 0.52f, 0.56f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.11f, 0.13f, 0.98f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.12f, 0.14f, 0.98f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.12f, 0.14f, 0.98f);
        colors[ImGuiCol_Border] = ImVec4(0.26f, 0.29f, 0.33f, 0.80f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.22f, 0.26f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.23f, 0.28f, 0.34f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.19f, 0.24f, 0.30f, 0.68f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.31f, 0.38f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.29f, 0.37f, 0.45f, 0.90f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.26f, 0.33f, 0.60f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.34f, 0.43f, 0.82f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.31f, 0.42f, 0.52f, 0.92f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.57f, 0.82f, 0.98f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.77f, 0.95f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.63f, 0.86f, 1.00f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.15f, 0.18f, 0.92f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.33f, 0.42f, 0.92f);
        colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.24f, 0.30f, 0.98f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.52f, 0.77f, 0.95f, 0.54f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.17f, 0.20f, 0.24f, 0.28f);

        style.WindowPadding = ImVec2(11.0f, 8.0f);
        style.FramePadding = ImVec2(9.0f, 6.0f);
        style.ItemSpacing = ImVec2(10.0f, 7.0f);
        style.SeparatorTextPadding = ImVec2(18.0f, style.FramePadding.y);
    }

    auto ApplyStyleScientificPaper(ImGuiStyle &style) -> void {
        style = ResetStyleLightBase();
        ApplyDeterministicMetrics(style, true);
        SetScientificPaperPalette(style.Colors);
        style.WindowRounding = 2.0f;
        style.ChildRounding = 2.0f;
        style.PopupRounding = 2.0f;
        style.FrameRounding = 2.0f;
        style.TabRounding = 2.0f;
    }

    auto ApplyStyleScientificSlate(ImGuiStyle &style) -> void {
        style = ResetStyleDarkBase();
        ApplyDeterministicMetrics(style);
        SetScientificSlatePalette(style.Colors);
    }

    auto ApplyStyleBlueprintNight(ImGuiStyle &style) -> void {
        style = ResetStyleDarkBase();
        ApplyDeterministicMetrics(style);
        SetBlueprintNightPalette(style.Colors);
        style.WindowRounding = 2.0f;
        style.ChildRounding = 2.0f;
        style.FrameRounding = 2.0f;
        style.TabRounding = 2.0f;
    }

    auto GetThemeDescriptors() -> const std::array<FThemeDescriptor, 4> & {
        static const std::array<FThemeDescriptor, 4> Themes = {{
            {ThemeIdStudioSlab, "StudioSlab", "GUI", ImVec4(0.07f, 0.08f, 0.10f, 1.0f), ApplyStyleStudioSlab},
            {ThemeIdScientificPaper, "Scientific Paper", "GUI", ImVec4(0.90f, 0.88f, 0.84f, 1.0f), ApplyStyleScientificPaper},
            {ThemeIdScientificSlate, "Scientific Slate", "GUI", ImVec4(0.06f, 0.09f, 0.11f, 1.0f), ApplyStyleScientificSlate},
            {ThemeIdBlueprintNight, "Blueprint Night", "GUI", ImVec4(0.02f, 0.06f, 0.11f, 1.0f), ApplyStyleBlueprintNight}
        }};
        return Themes;
    }

    auto FindThemeDescriptor(const Slab::Str &themeId) -> const FThemeDescriptor & {
        const auto &themes = GetThemeDescriptors();
        const auto it = std::find_if(themes.begin(), themes.end(), [&](const auto &theme) {
            return theme.ThemeId == themeId;
        });
        if (it != themes.end()) return *it;
        return themes.front();
    }

    auto Shutdown(FWorkspaceSandboxApp &app) -> void {
        if (app.Window == nullptr) return;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(app.Window);
        app.Window = nullptr;
        glfwTerminate();
    }

    auto GetWorkspaceDefinitions() -> const std::array<WindowingV2::FWorkspaceDefinitionV2, 3> & {
        static const std::array<WindowingV2::FWorkspaceDefinitionV2, 3> Workspaces = {{
            {WorkspaceIdExplore, "Explore", false},
            {WorkspaceIdPlots, "Plots", false},
            {WorkspaceIdAssets, "Assets", false}
        }};
        return Workspaces;
    }

    auto FindWorkspaceDefinition(const Slab::Str &workspaceId) -> const WindowingV2::FWorkspaceDefinitionV2 & {
        const auto &workspaces = GetWorkspaceDefinitions();
        const auto it = std::find_if(workspaces.begin(), workspaces.end(), [&](const auto &workspace) {
            return workspace.WorkspaceId == workspaceId;
        });
        if (it != workspaces.end()) return *it;
        return workspaces.front();
    }

    auto AppendEvent(FWorkspaceSandboxApp &app, Slab::Str message) -> void {
        app.EventLog.push_back(std::move(message));
        constexpr std::size_t MaxLogEntries = 14;
        if (app.EventLog.size() > MaxLogEntries) {
            app.EventLog.erase(app.EventLog.begin(), app.EventLog.begin() + 1);
        }
    }

    auto ApplyTheme(FWorkspaceSandboxApp &app, const Slab::Str &themeId, const bool logEvent = true) -> void {
        const auto &theme = FindThemeDescriptor(themeId);
        ImGuiStyle style;
        if (theme.ApplyStyle != nullptr) {
            theme.ApplyStyle(style);
            ImGui::GetStyle() = style;
        }

        app.ActiveThemeId = theme.ThemeId;
        app.ClearColor = theme.ClearColor;

        if (logEvent) {
            AppendEvent(app, Slab::Str("Applied theme ") + theme.DisplayName);
        }
    }

    auto BuildVisibilityItems(FWorkspaceSandboxApp &app) -> std::vector<WindowingV2::FWorkspaceVisibilityItemV2> {
        std::vector<WindowingV2::FWorkspaceVisibilityItemV2> items;
        items.reserve(10);

        const auto addItem = [&](const char *itemId,
                                 const char *workspaceId,
                                 const char *label,
                                 bool *bVisible) {
            items.push_back(WindowingV2::FWorkspaceVisibilityItemV2{
                itemId,
                workspaceId,
                label,
                bVisible,
                true
            });
        };

        addItem("scene_outline", WorkspaceIdExplore, "Outline", &app.bShowSceneOutline);
        addItem("viewport_preview", WorkspaceIdExplore, "Preview", &app.bShowViewportPreview);
        addItem("inspector", WorkspaceIdExplore, "Inspector", &app.bShowInspector);
        addItem("console", WorkspaceIdExplore, "Console", &app.bShowConsole);

        addItem("series_controls", WorkspaceIdPlots, "Controls", &app.bShowSeriesControls);
        addItem("plot_surface", WorkspaceIdPlots, "Surface", &app.bShowPlotSurface);
        addItem("metrics", WorkspaceIdPlots, "Metrics", &app.bShowMetrics);

        addItem("asset_browser", WorkspaceIdAssets, "Browser", &app.bShowAssetBrowser);
        addItem("preview_board", WorkspaceIdAssets, "Board", &app.bShowPreviewBoard);
        addItem("details", WorkspaceIdAssets, "Details", &app.bShowDetails);

        return items;
    }

    auto SaveWorkspaceVisibility(FWorkspaceSandboxApp &app) -> void {
        const auto items = BuildVisibilityItems(app);
        WindowingV2::SaveWorkspaceVisibilityState(app.WorkspaceShellState, items, app.ActiveWorkspaceId);
    }

    auto LoadWorkspaceVisibility(FWorkspaceSandboxApp &app) -> void {
        const auto items = BuildVisibilityItems(app);
        WindowingV2::LoadWorkspaceVisibilityState(app.WorkspaceShellState, items, app.ActiveWorkspaceId);
    }

    auto SetActiveWorkspace(FWorkspaceSandboxApp &app, const Slab::Str &workspaceId) -> void {
        if (workspaceId.empty() || workspaceId == app.ActiveWorkspaceId) return;

        SaveWorkspaceVisibility(app);
        app.ActiveWorkspaceId = workspaceId;
        LoadWorkspaceVisibility(app);
        AppendEvent(app, Slab::Str("Switched workspace to ") + FindWorkspaceDefinition(workspaceId).DisplayName);
    }

    auto BuildWorkspaceLayout(const Slab::Str &workspaceId) -> WindowingV2::FWorkspaceDockLayoutV2 {
        using WindowingV2::EDockSplitDirectionV2;
        using WindowingV2::FDockNodeSplitV2;
        using WindowingV2::FDockWindowPlacementV2;

        WindowingV2::FWorkspaceDockLayoutV2 layout;

        if (workspaceId == WorkspaceIdExplore) {
            layout.Splits = {
                FDockNodeSplitV2{"main", "dock_left", "main", EDockSplitDirectionV2::Left, 0.22f},
                FDockNodeSplitV2{"main", "dock_right", "main", EDockSplitDirectionV2::Right, 0.28f},
                FDockNodeSplitV2{"main", "dock_bottom", "main", EDockSplitDirectionV2::Down, 0.30f}
            };
            layout.Placements = {
                FDockWindowPlacementV2{WindowTitleSceneOutline, "dock_left"},
                FDockWindowPlacementV2{WindowTitleViewportPreview, "main"},
                FDockWindowPlacementV2{WindowTitleInspector, "dock_right"},
                FDockWindowPlacementV2{WindowTitleConsole, "dock_bottom"}
            };
            return layout;
        }

        if (workspaceId == WorkspaceIdPlots) {
            layout.Splits = {
                FDockNodeSplitV2{"main", "dock_left", "main", EDockSplitDirectionV2::Left, 0.24f},
                FDockNodeSplitV2{"main", "dock_bottom", "main", EDockSplitDirectionV2::Down, 0.30f}
            };
            layout.Placements = {
                FDockWindowPlacementV2{WindowTitleSeriesControls, "dock_left"},
                FDockWindowPlacementV2{WindowTitlePlotSurface, "main"},
                FDockWindowPlacementV2{WindowTitleMetrics, "dock_bottom"}
            };
            return layout;
        }

        layout.Splits = {
            FDockNodeSplitV2{"main", "dock_left", "main", EDockSplitDirectionV2::Left, 0.24f},
            FDockNodeSplitV2{"main", "dock_right", "main", EDockSplitDirectionV2::Right, 0.30f}
        };
        layout.Placements = {
            FDockWindowPlacementV2{WindowTitleAssetBrowser, "dock_left"},
            FDockWindowPlacementV2{WindowTitlePreviewBoard, "main"},
            FDockWindowPlacementV2{WindowTitleDetails, "dock_right"}
        };
        return layout;
    }

    auto DrawPreviewSurface(FWorkspaceSandboxApp &app,
                            const char *canvasId,
                            const ImVec4 &accentColor,
                            const char *caption) -> void {
        ImGui::TextDisabled("%s", caption);
        ImGui::Separator();

        ImVec2 size = ImGui::GetContentRegionAvail();
        size.x = std::max(size.x, 180.0f);
        size.y = std::max(size.y, 220.0f);

        const auto cursor = ImGui::GetCursorScreenPos();
        const auto min = cursor;
        const auto max = ImVec2(cursor.x + size.x, cursor.y + size.y);
        auto *drawList = ImGui::GetWindowDrawList();

        drawList->AddRectFilled(min, max, IM_COL32(14, 18, 26, 255), 12.0f);
        drawList->AddRect(min, max, IM_COL32(70, 86, 106, 255), 12.0f, 0, 1.5f);

        if (app.bShowGrid) {
            for (int i = 1; i < 8; ++i) {
                const float x = min.x + (size.x * static_cast<float>(i) / 8.0f);
                const float y = min.y + (size.y * static_cast<float>(i) / 8.0f);
                drawList->AddLine(ImVec2(x, min.y), ImVec2(x, max.y), IM_COL32(34, 41, 52, 255), 1.0f);
                drawList->AddLine(ImVec2(min.x, y), ImVec2(max.x, y), IM_COL32(34, 41, 52, 255), 1.0f);
            }
        }

        const float phase = static_cast<float>(ImGui::GetTime()) * (app.bAnimatePreview ? 1.0f : 0.0f);
        const float amplitude = (size.y * 0.24f) * app.PreviewAmplitude;
        const float baseline = min.y + size.y * 0.50f;
        const float frequency = std::max(0.2f, app.PreviewFrequency);
        const float zoom = std::max(0.25f, app.PreviewZoom);

        std::vector<ImVec2> primary;
        std::vector<ImVec2> secondary;
        constexpr int SampleCount = 128;
        primary.reserve(SampleCount);
        secondary.reserve(SampleCount);

        for (int i = 0; i < SampleCount; ++i) {
            const float t = static_cast<float>(i) / static_cast<float>(SampleCount - 1);
            const float x = min.x + size.x * t;
            const float angle = (t * frequency * 6.2831853f * zoom) + phase;
            const float yPrimary = baseline + std::sin(angle) * amplitude;
            const float ySecondary = baseline + std::cos(angle * 0.68f + 0.6f) * amplitude * 0.46f;
            primary.emplace_back(x, yPrimary);
            secondary.emplace_back(x, ySecondary);
        }

        drawList->AddPolyline(
            primary.data(),
            static_cast<int>(primary.size()),
            ImGui::ColorConvertFloat4ToU32(accentColor),
            ImDrawFlags_None,
            3.0f);
        drawList->AddPolyline(
            secondary.data(),
            static_cast<int>(secondary.size()),
            IM_COL32(140, 166, 190, 220),
            ImDrawFlags_None,
            2.0f);

        if (app.bShowMarkers) {
            for (int i = 10; i < SampleCount; i += 24) {
                drawList->AddCircleFilled(primary[static_cast<std::size_t>(i)], 4.0f, IM_COL32(244, 196, 118, 255));
            }
        }

        drawList->AddText(
            ImVec2(min.x + 16.0f, min.y + 14.0f),
            IM_COL32(220, 226, 232, 255),
            "browser-safe preview surface");

        ImGui::InvisibleButton(canvasId, size);
    }

    auto BuildPanelRegistry(FWorkspaceSandboxApp &app) -> std::vector<WindowingV2::FPanelSurfaceRegistrationV2> {
        std::vector<WindowingV2::FPanelSurfaceRegistrationV2> panels;
        panels.reserve(10);

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "scene_outline",
            WindowTitleSceneOutline,
            WorkspaceIdExplore,
            &app.bShowSceneOutline,
            false,
            false,
            [&app]() {
                ImGui::TextDisabled("Workspace sample");
                ImGui::Separator();
                ImGui::BulletText("Root scene");
                ImGui::Indent();
                ImGui::Selectable("Camera Rig", true);
                ImGui::Selectable("Sine Generator", false);
                ImGui::Selectable("Diagnostics", false);
                ImGui::Unindent();
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "viewport_preview",
            WindowTitleViewportPreview,
            WorkspaceIdExplore,
            &app.bShowViewportPreview,
            false,
            false,
            [&app]() {
                DrawPreviewSurface(app, "##ExplorePreview", ImVec4(0.33f, 0.71f, 0.92f, 1.0f), "Explorer viewport");
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "inspector",
            WindowTitleInspector,
            WorkspaceIdExplore,
            &app.bShowInspector,
            false,
            false,
            [&app]() {
                ImGui::Text("Selection");
                ImGui::Separator();
                ImGui::TextUnformatted("Camera Rig");
                ImGui::SliderFloat("Preview zoom", &app.PreviewZoom, 0.50f, 2.20f);
                ImGui::SliderFloat("Amplitude", &app.PreviewAmplitude, 0.20f, 1.20f);
                ImGui::Checkbox("Animate surface", &app.bAnimatePreview);
                ImGui::Checkbox("Show grid", &app.bShowGrid);
                ImGui::Checkbox("Show markers", &app.bShowMarkers);
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "console",
            WindowTitleConsole,
            WorkspaceIdExplore,
            &app.bShowConsole,
            false,
            false,
            [&app]() {
                if (ImGui::Button("Append sample event")) {
                    AppendEvent(app, "Manual event injected from console");
                }
                ImGui::Separator();
                if (ImGui::BeginChild("##EventLog")) {
                    for (const auto &line : app.EventLog) {
                        ImGui::TextUnformatted(line.c_str());
                    }
                }
                ImGui::EndChild();
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "series_controls",
            WindowTitleSeriesControls,
            WorkspaceIdPlots,
            &app.bShowSeriesControls,
            false,
            false,
            [&app]() {
                const char *series[] = {"Primary", "Secondary", "Envelope"};
                ImGui::Text("Series");
                ImGui::Separator();
                ImGui::Combo("Focus", &app.SelectedSeriesIndex, series, IM_ARRAYSIZE(series));
                ImGui::SliderFloat("Frequency", &app.PreviewFrequency, 0.40f, 3.60f);
                ImGui::SliderFloat("Zoom", &app.PreviewZoom, 0.50f, 2.60f);
                ImGui::Checkbox("Markers", &app.bShowMarkers);
                ImGui::Checkbox("Animate", &app.bAnimatePreview);
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "plot_surface",
            WindowTitlePlotSurface,
            WorkspaceIdPlots,
            &app.bShowPlotSurface,
            false,
            false,
            [&app]() {
                DrawPreviewSurface(app, "##PlotSurface", ImVec4(0.94f, 0.58f, 0.22f, 1.0f), "Interactive plot placeholder");
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "metrics",
            WindowTitleMetrics,
            WorkspaceIdPlots,
            &app.bShowMetrics,
            false,
            false,
            [&app]() {
                const auto &io = ImGui::GetIO();
                if (ImGui::BeginTable("##PlotMetrics", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted("FPS");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%.1f", io.Framerate);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted("Series");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d", app.SelectedSeriesIndex + 1);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted("Dockspace");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%u", app.WorkspaceShellState.ActiveDockspaceId);
                    ImGui::EndTable();
                }
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "asset_browser",
            WindowTitleAssetBrowser,
            WorkspaceIdAssets,
            &app.bShowAssetBrowser,
            false,
            false,
            [&app]() {
                static const std::array<const char *, 5> Assets = {{
                    "mock.signal.wave",
                    "mock.signal.noise",
                    "mock.material.copper",
                    "mock.study.bundle",
                    "mock.snapshot.frame"
                }};

                for (int i = 0; i < static_cast<int>(Assets.size()); ++i) {
                    if (ImGui::Selectable(Assets[static_cast<std::size_t>(i)], app.SelectedAssetIndex == i)) {
                        app.SelectedAssetIndex = i;
                        AppendEvent(app, Slab::Str("Selected asset ") + Assets[static_cast<std::size_t>(i)]);
                    }
                }
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "preview_board",
            WindowTitlePreviewBoard,
            WorkspaceIdAssets,
            &app.bShowPreviewBoard,
            false,
            false,
            [&app]() {
                DrawPreviewSurface(app, "##PreviewBoard", ImVec4(0.46f, 0.80f, 0.48f, 1.0f), "Preview board");
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "details",
            WindowTitleDetails,
            WorkspaceIdAssets,
            &app.bShowDetails,
            false,
            false,
            [&app]() {
                static const std::array<const char *, 5> AssetLabels = {{
                    "Signal Wave",
                    "Signal Noise",
                    "Material Copper",
                    "Study Bundle",
                    "Snapshot Frame"
                }};

                const int selected = std::clamp(app.SelectedAssetIndex, 0, static_cast<int>(AssetLabels.size()) - 1);
                ImGui::Text("Selected asset");
                ImGui::Separator();
                ImGui::TextUnformatted(AssetLabels[static_cast<std::size_t>(selected)]);
                ImGui::TextDisabled("Browser-hosted workspace sample");
                ImGui::Spacing();
                ImGui::BulletText("Stable dock layout");
                ImGui::BulletText("Top tabs + workspace strip");
                ImGui::BulletText("Shared shell code reused directly");
            }
        });

        return panels;
    }

    auto RenderFrame(void *userData) -> void {
        auto *app = static_cast<FWorkspaceSandboxApp *>(userData);
        if (app == nullptr || app->Window == nullptr) return;

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (app->bShowImGuiDemo) {
            ImGui::ShowDemoWindow(&app->bShowImGuiDemo);
        }

        const auto &workspaces = GetWorkspaceDefinitions();
        const auto visibilityItems = BuildVisibilityItems(*app);
        const auto panelRegistry = BuildPanelRegistry(*app);
        const auto &activeWorkspace = FindWorkspaceDefinition(app->ActiveWorkspaceId);

        WindowingV2::DrawWorkspaceLauncher(
            app->WorkspaceShellState,
            0.0f,
            [app](const ImVec2 &buttonSize) {
                if (ImGui::Button(FindThemeDescriptor(app->ActiveThemeId).LauncherLabel, buttonSize)) {
                    ImGui::OpenPopup("##WasmWorkspaceThemePopup");
                }

                if (ImGui::BeginPopup("##WasmWorkspaceThemePopup")) {
                    ImGui::TextUnformatted("GUI Theme");
                    ImGui::Separator();

                    for (const auto &theme : GetThemeDescriptors()) {
                        const bool bSelected = app->ActiveThemeId == theme.ThemeId;
                        if (ImGui::Selectable(theme.DisplayName, bSelected)) {
                            ApplyTheme(*app, theme.ThemeId);
                        }
                        if (bSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }

                    ImGui::Separator();
                    if (ImGui::MenuItem("Show Dear ImGui demo", nullptr, app->bShowImGuiDemo)) {
                        app->bShowImGuiDemo = !app->bShowImGuiDemo;
                        AppendEvent(*app, app->bShowImGuiDemo ? "Opened Dear ImGui demo" : "Closed Dear ImGui demo");
                    }

                    ImGui::EndPopup();
                }
            },
            []() {},
            "##WasmWorkspaceLauncher");

        WindowingV2::DrawWorkspaceTabs(
            app->WorkspaceShellState,
            workspaces,
            app->ActiveWorkspaceId,
            0.0f,
            [app](const Slab::Str &workspaceId) {
                SetActiveWorkspace(*app, workspaceId);
            },
            []() {},
            "##WasmWorkspaceTabs");

        WindowingV2::DrawWorkspaceStrip(
            app->WorkspaceShellState,
            activeWorkspace,
            visibilityItems,
            0.0f,
            []() {},
            "##WasmWorkspaceStrip");

        WindowingV2::DrawDockspaceHost(
            app->WorkspaceShellState,
            workspaces,
            app->ActiveWorkspaceId,
            0.0f,
            [](const Slab::Str &workspaceId) {
                return BuildWorkspaceLayout(workspaceId);
            },
            [](const Slab::Str &, const std::map<Slab::Str, ImGuiID> &) {},
            []() {},
            "##WasmWorkspaceDockspaceHost",
            "##WasmWorkspaceDockspace-");

        WindowingV2::DrawDockedToolWindows(
            panelRegistry,
            app->ActiveWorkspaceId,
            true);
        SaveWorkspaceVisibility(*app);

        ImGui::Render();

        int framebufferWidth = 1;
        int framebufferHeight = 1;
        glfwGetFramebufferSize(app->Window, &framebufferWidth, &framebufferHeight);

        glViewport(0, 0, framebufferWidth, framebufferHeight);
        glClearColor(app->ClearColor.x, app->ClearColor.y, app->ClearColor.z, app->ClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(app->Window);
    }

} // namespace

auto main() -> int {
    glfwSetErrorCallback(ErrorCallback);
    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to initialize GLFW.\n");
        return 1;
    }

    constexpr const char *glslVersion = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

    FWorkspaceSandboxApp app{};
    app.Window = glfwCreateWindow(1440, 900, "StudioSlab WASM Workspace Sandbox", nullptr, nullptr);
    if (app.Window == nullptr) {
        std::fprintf(stderr, "Failed to create GLFW window.\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(app.Window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = nullptr;

    ApplyTheme(app, app.ActiveThemeId, false);

    ImGui_ImplGlfw_InitForOpenGL(app.Window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(app.Window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glslVersion);

    LoadWorkspaceVisibility(app);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(RenderFrame, &app, 0, true);
    return 0;
#else
    while (!glfwWindowShouldClose(app.Window)) {
        RenderFrame(&app);
    }

    Shutdown(app);
    return 0;
#endif
}
