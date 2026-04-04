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
#include <cstdint>
#include <cstdio>
#include <random>
#include <string>
#include <vector>

namespace {

    namespace WindowingV2 = Slab::Graphics::Windowing::V2;

    constexpr auto ThemeIdStudioSlab = "studio_slab";
    constexpr auto ThemeIdScientificPaper = "scientific_paper";
    constexpr auto ThemeIdScientificSlate = "scientific_slate";
    constexpr auto ThemeIdBlueprintNight = "blueprint_night";

#if defined(STUDIOSLAB_WASM_ISING_WORKSPACE)
    constexpr auto WorkspaceIdSimulation = "simulation";
    constexpr auto WorkspaceIdObservables = "observables";
    constexpr auto WorkspaceIdStudy = "study";

    constexpr auto WindowTitleIsingControls = "Ising Controls";
    constexpr auto WindowTitleSpinLattice = "Spin Lattice";
    constexpr auto WindowTitleRunInspector = "Run Inspector";
    constexpr auto WindowTitleEventConsole = "Event Console";

    constexpr auto WindowTitleSamplingControls = "Sampling Controls";
    constexpr auto WindowTitleObservableHistory = "Observable History";
    constexpr auto WindowTitleSweepStatistics = "Sweep Statistics";

    constexpr auto WindowTitlePresetBrowser = "Preset Browser";
    constexpr auto WindowTitlePhaseSnapshot = "Phase Snapshot";
    constexpr auto WindowTitleHysteresisTrace = "Hysteresis Trace";
    constexpr auto WindowTitleModelNotes = "Model Notes";
#else
    constexpr auto WorkspaceIdExplore = "explore";
    constexpr auto WorkspaceIdPlots = "plots";
    constexpr auto WorkspaceIdAssets = "assets";

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
#endif

#if defined(STUDIOSLAB_WASM_ISING_WORKSPACE)
    struct FIsingMetropolisStateWasm {
        int L = 64;
        double Temperature = 2.269185314;
        double ExternalField = 0.0;
        bool bFerromagneticInitial = false;
        bool bRunning = true;
        bool bPendingReset = true;
        int SweepsPerFrame = 1;
        unsigned int Seed = 1337u;
        std::mt19937 Rng = std::mt19937(Seed);
        std::vector<std::int8_t> Spins;
        std::uint64_t SweepCount = 0;
        double EnergyDensity = 0.0;
        double Magnetization = 0.0;
        double AcceptanceRatio = 0.0;
        unsigned int AcceptedLastSweep = 0;
        unsigned int RejectedLastSweep = 0;
        int HoveredI = -1;
        int HoveredJ = -1;
        int HistoryPointBudget = 240;
        std::vector<float> MagnetizationHistory;
        std::vector<float> EnergyHistory;
        std::vector<float> AcceptanceHistory;
        std::vector<float> ExternalFieldHistory;
        std::vector<float> MagnetizationFieldHistory;
    };
#endif

    struct FWorkspaceSandboxApp {
        GLFWwindow *Window = nullptr;
        ImVec4 ClearColor = ImVec4(0.06f, 0.08f, 0.11f, 1.0f);
        WindowingV2::FWorkspaceShellStateV2 WorkspaceShellState;
        Slab::Str ActiveThemeId = ThemeIdStudioSlab;
#if defined(STUDIOSLAB_WASM_ISING_WORKSPACE)
        Slab::Str ActiveWorkspaceId = WorkspaceIdSimulation;
        FIsingMetropolisStateWasm Ising;

        bool bShowIsingControls = true;
        bool bShowSpinLattice = true;
        bool bShowRunInspector = true;
        bool bShowEventConsole = true;

        bool bShowSamplingControls = true;
        bool bShowObservableHistory = true;
        bool bShowSweepStatistics = true;

        bool bShowPresetBrowser = true;
        bool bShowPhaseSnapshot = true;
        bool bShowHysteresisTrace = true;
        bool bShowModelNotes = true;
#else
        Slab::Str ActiveWorkspaceId = WorkspaceIdExplore;

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
#endif

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
#if defined(STUDIOSLAB_WASM_ISING_WORKSPACE)
        static const std::array<WindowingV2::FWorkspaceDefinitionV2, 3> Workspaces = {{
            {WorkspaceIdSimulation, "Simulation", false},
            {WorkspaceIdObservables, "Observables", false},
            {WorkspaceIdStudy, "Study", false}
        }};
#else
        static const std::array<WindowingV2::FWorkspaceDefinitionV2, 3> Workspaces = {{
            {WorkspaceIdExplore, "Explore", false},
            {WorkspaceIdPlots, "Plots", false},
            {WorkspaceIdAssets, "Assets", false}
        }};
#endif
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

#if defined(STUDIOSLAB_WASM_ISING_WORKSPACE)
    [[nodiscard]] auto ClampIsingHistoryPointBudget(const int value) -> int {
        return std::clamp(value, 32, 960);
    }

    auto TrimIsingHistorySample(std::vector<float> &history, const std::size_t maxHistorySamples) -> void {
        if (history.size() > maxHistorySamples) {
            history.erase(
                history.begin(),
                history.begin() + static_cast<std::vector<float>::difference_type>(history.size() - maxHistorySamples));
        }
    }

    auto TrimAllIsingHistories(FIsingMetropolisStateWasm &ising) -> void {
        const auto maxHistorySamples = static_cast<std::size_t>(ClampIsingHistoryPointBudget(ising.HistoryPointBudget));
        ising.HistoryPointBudget = static_cast<int>(maxHistorySamples);
        TrimIsingHistorySample(ising.MagnetizationHistory, maxHistorySamples);
        TrimIsingHistorySample(ising.EnergyHistory, maxHistorySamples);
        TrimIsingHistorySample(ising.AcceptanceHistory, maxHistorySamples);
        TrimIsingHistorySample(ising.ExternalFieldHistory, maxHistorySamples);
        TrimIsingHistorySample(ising.MagnetizationFieldHistory, maxHistorySamples);
    }

    auto PushIsingHistorySample(std::vector<float> &history,
                                const float value,
                                const std::size_t maxHistorySamples) -> void {
        history.push_back(value);
        TrimIsingHistorySample(history, maxHistorySamples);
    }

    [[nodiscard]] auto ClampIsingLatticeSize(const int value) -> int {
        // Keep the browser sandbox below the reported failure threshold until the
        // larger-lattice issue is traced in the wasm runtime path.
        return std::clamp(value, 8, 127);
    }

    [[nodiscard]] auto IsingCellIndex(const FIsingMetropolisStateWasm &ising, const int i, const int j) -> std::size_t {
        return static_cast<std::size_t>(j) * static_cast<std::size_t>(ising.L) + static_cast<std::size_t>(i);
    }

    [[nodiscard]] auto IsingSpinAt(const FIsingMetropolisStateWasm &ising, int i, int j) -> int {
        if (ising.L <= 0 || ising.Spins.empty()) return 0;
        i = (i % ising.L + ising.L) % ising.L;
        j = (j % ising.L + ising.L) % ising.L;
        return static_cast<int>(ising.Spins[IsingCellIndex(ising, i, j)]);
    }

    [[nodiscard]] auto ComputeIsingNeighborSum(const FIsingMetropolisStateWasm &ising, const int i, const int j) -> int {
        return IsingSpinAt(ising, i, j - 1)
             + IsingSpinAt(ising, i, j + 1)
             + IsingSpinAt(ising, i + 1, j)
             + IsingSpinAt(ising, i - 1, j);
    }

    auto MeasureIsingState(FIsingMetropolisStateWasm &ising,
                           const unsigned int accepted,
                           const unsigned int rejected) -> void {
        const auto latticeSize = static_cast<double>(ising.L) * static_cast<double>(ising.L);
        if (latticeSize <= 0.0 || ising.Spins.empty()) {
            ising.EnergyDensity = 0.0;
            ising.Magnetization = 0.0;
            ising.AcceptanceRatio = 0.0;
            ising.AcceptedLastSweep = accepted;
            ising.RejectedLastSweep = rejected;
            return;
        }

        double energy = 0.0;
        double magnetization = 0.0;
        for (int j = 0; j < ising.L; ++j) {
            for (int i = 0; i < ising.L; ++i) {
                const auto s = static_cast<double>(IsingSpinAt(ising, i, j));
                const auto east = static_cast<double>(IsingSpinAt(ising, i + 1, j));
                const auto south = static_cast<double>(IsingSpinAt(ising, i, j + 1));
                energy -= s * (east + south);
                energy -= ising.ExternalField * s;
                magnetization += s;
            }
        }

        ising.EnergyDensity = energy / latticeSize;
        ising.Magnetization = magnetization / latticeSize;
        ising.AcceptanceRatio = static_cast<double>(accepted) / latticeSize;
        ising.AcceptedLastSweep = accepted;
        ising.RejectedLastSweep = rejected;

        const auto maxHistorySamples = static_cast<std::size_t>(ClampIsingHistoryPointBudget(ising.HistoryPointBudget));
        ising.HistoryPointBudget = static_cast<int>(maxHistorySamples);

        PushIsingHistorySample(ising.MagnetizationHistory, static_cast<float>(ising.Magnetization), maxHistorySamples);
        PushIsingHistorySample(ising.EnergyHistory, static_cast<float>(ising.EnergyDensity), maxHistorySamples);
        PushIsingHistorySample(ising.AcceptanceHistory, static_cast<float>(ising.AcceptanceRatio), maxHistorySamples);
        PushIsingHistorySample(ising.ExternalFieldHistory, static_cast<float>(ising.ExternalField), maxHistorySamples);
        PushIsingHistorySample(ising.MagnetizationFieldHistory, static_cast<float>(ising.Magnetization), maxHistorySamples);
    }

    auto ResetIsingState(FWorkspaceSandboxApp &app, const bool logEvent = true) -> void {
        auto &ising = app.Ising;
        ising.L = ClampIsingLatticeSize(ising.L);
        ising.SweepsPerFrame = std::clamp(ising.SweepsPerFrame, 1, 32);
        ising.Rng.seed(ising.Seed);
        ising.Spins.assign(static_cast<std::size_t>(ising.L) * static_cast<std::size_t>(ising.L), 1);

        std::bernoulli_distribution spinPick(0.5);
        if (!ising.bFerromagneticInitial) {
            for (auto &spin : ising.Spins) {
                spin = spinPick(ising.Rng) ? std::int8_t(1) : std::int8_t(-1);
            }
        }

        ising.SweepCount = 0;
        ising.HoveredI = -1;
        ising.HoveredJ = -1;
        ising.MagnetizationHistory.clear();
        ising.EnergyHistory.clear();
        ising.AcceptanceHistory.clear();
        ising.ExternalFieldHistory.clear();
        ising.MagnetizationFieldHistory.clear();
        ising.bPendingReset = false;
        MeasureIsingState(ising, 0, 0);

        if (logEvent) {
            AppendEvent(
                app,
                Slab::Str("Reset Ising lattice to ")
                    + std::to_string(ising.L) + "x" + std::to_string(ising.L)
                    + (ising.bFerromagneticInitial ? " ferromagnetic" : " random"));
        }
    }

    auto RunIsingSweep(FIsingMetropolisStateWasm &ising) -> void {
        if (ising.Spins.empty() || ising.L < 2) return;

        std::uniform_int_distribution<int> pickCell(0, ising.L - 1);
        std::uniform_real_distribution<double> pickUnit(0.0, 1.0);

        unsigned int accepted = 0;
        unsigned int rejected = 0;
        const auto attempts = ising.L * ising.L;

        for (int trial = 0; trial < attempts; ++trial) {
            const auto i = pickCell(ising.Rng);
            const auto j = pickCell(ising.Rng);
            const auto s = static_cast<double>(IsingSpinAt(ising, i, j));
            const auto neighborSum = static_cast<double>(ComputeIsingNeighborSum(ising, i, j));
            const auto deltaEnergy = 2.0 * s * (neighborSum + ising.ExternalField);

            bool accept = deltaEnergy <= 0.0;
            if (!accept && ising.Temperature > 0.0) {
                const auto boltzmann = std::exp(-deltaEnergy / ising.Temperature);
                accept = pickUnit(ising.Rng) < std::min(1.0, boltzmann);
            }

            if (accept) {
                auto &spin = ising.Spins[IsingCellIndex(ising, i, j)];
                spin = static_cast<std::int8_t>(-spin);
                ++accepted;
            } else {
                ++rejected;
            }
        }

        ++ising.SweepCount;
        MeasureIsingState(ising, accepted, rejected);
    }

    auto TickIsingSimulation(FWorkspaceSandboxApp &app) -> void {
        if (app.Ising.Spins.empty()) {
            ResetIsingState(app, false);
        }

        if (!app.Ising.bRunning || app.Ising.bPendingReset) return;
        for (int sweep = 0; sweep < app.Ising.SweepsPerFrame; ++sweep) {
            RunIsingSweep(app.Ising);
        }
    }

    auto ApplyIsingPreset(FWorkspaceSandboxApp &app,
                          const int latticeSize,
                          const double temperature,
                          const double externalField,
                          const bool bFerromagneticInitial,
                          const int sweepsPerFrame,
                          const char *label) -> void {
        app.Ising.L = ClampIsingLatticeSize(latticeSize);
        app.Ising.Temperature = temperature;
        app.Ising.ExternalField = externalField;
        app.Ising.bFerromagneticInitial = bFerromagneticInitial;
        app.Ising.SweepsPerFrame = std::clamp(sweepsPerFrame, 1, 32);
        app.Ising.bPendingReset = true;
        ResetIsingState(app, false);
        AppendEvent(app, Slab::Str("Applied Ising preset ") + label);
    }

    auto DrawIsingPresetControls(FWorkspaceSandboxApp &app) -> void {
        if (ImGui::Selectable("Ordered cold phase")) {
            ApplyIsingPreset(app, 64, 1.20, 0.00, true, 1, "Ordered cold phase");
        }
        if (ImGui::Selectable("Critical drift")) {
            ApplyIsingPreset(app, 96, 2.269185314, 0.00, false, 1, "Critical drift");
        }
        if (ImGui::Selectable("Hot disorder")) {
            ApplyIsingPreset(app, 64, 3.60, 0.00, false, 2, "Hot disorder");
        }
        if (ImGui::Selectable("Field biased")) {
            ApplyIsingPreset(app, 64, 2.00, 0.35, false, 1, "Field biased");
        }
    }

    auto DrawIsingLatticeSurface(FWorkspaceSandboxApp &app,
                                 const char *canvasId,
                                 const char *caption) -> void {
        ImGui::TextDisabled("%s", caption);
        ImGui::Separator();

        ImVec2 size = ImGui::GetContentRegionAvail();
        const float side = std::max(220.0f, std::min(size.x, size.y > 32.0f ? size.y - 28.0f : size.x));
        size = ImVec2(side, side);

        const auto cursor = ImGui::GetCursorScreenPos();
        const auto min = cursor;
        const auto max = ImVec2(cursor.x + size.x, cursor.y + size.y);
        auto *drawList = ImGui::GetWindowDrawList();

        drawList->AddRectFilled(min, max, ImGui::GetColorU32(ImGuiCol_FrameBg), 8.0f);
        drawList->AddRect(min, max, ImGui::GetColorU32(ImGuiCol_Border), 8.0f, 0, 1.0f);

        auto &ising = app.Ising;
        if (!ising.Spins.empty() && ising.L > 0) {
            const float cellSize = size.x / static_cast<float>(ising.L);
            const auto spinUpColor = IM_COL32(243, 191, 92, 255);
            const auto spinDownColor = IM_COL32(89, 155, 228, 255);

            for (int j = 0; j < ising.L; ++j) {
                for (int i = 0; i < ising.L; ++i) {
                    const auto x0 = min.x + static_cast<float>(i) * cellSize;
                    const auto y0 = min.y + static_cast<float>(j) * cellSize;
                    const auto x1 = min.x + static_cast<float>(i + 1) * cellSize;
                    const auto y1 = min.y + static_cast<float>(j + 1) * cellSize;
                    const auto color = IsingSpinAt(ising, i, j) > 0 ? spinUpColor : spinDownColor;
                    drawList->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color);
                }
            }

            if (ising.L <= 40) {
                const auto gridColor = IM_COL32(16, 18, 24, 48);
                for (int index = 1; index < ising.L; ++index) {
                    const auto offset = static_cast<float>(index) * cellSize;
                    drawList->AddLine(ImVec2(min.x + offset, min.y), ImVec2(min.x + offset, max.y), gridColor, 1.0f);
                    drawList->AddLine(ImVec2(min.x, min.y + offset), ImVec2(max.x, min.y + offset), gridColor, 1.0f);
                }
            }
        } else {
            drawList->AddText(
                ImVec2(min.x + 14.0f, min.y + 12.0f),
                ImGui::GetColorU32(ImGuiCol_TextDisabled),
                "Reset the lattice to initialize the browser Ising state.");
        }

        ImGui::InvisibleButton(canvasId, size);
        if (ImGui::IsItemHovered() && !ising.Spins.empty() && ising.L > 0) {
            const auto mouse = ImGui::GetIO().MousePos;
            const float localX = std::clamp(mouse.x - min.x, 0.0f, size.x - 0.001f);
            const float localY = std::clamp(mouse.y - min.y, 0.0f, size.y - 0.001f);
            ising.HoveredI = std::clamp(static_cast<int>(localX / (size.x / static_cast<float>(ising.L))), 0, ising.L - 1);
            ising.HoveredJ = std::clamp(static_cast<int>(localY / (size.y / static_cast<float>(ising.L))), 0, ising.L - 1);
        } else if (!ImGui::IsItemHovered()) {
            ising.HoveredI = -1;
            ising.HoveredJ = -1;
        }

        ImGui::TextDisabled(
            "sweeps: %llu  |  acceptance: %.3f  |  magnetization: %.3f",
            static_cast<unsigned long long>(ising.SweepCount),
            ising.AcceptanceRatio,
            ising.Magnetization);
    }

    auto DrawPhaseSnapshotSurface(FWorkspaceSandboxApp &app,
                                  const char *canvasId,
                                  const char *caption) -> void {
        ImGui::TextDisabled("%s", caption);
        ImGui::Separator();

        ImVec2 size = ImGui::GetContentRegionAvail();
        size.x = std::max(size.x, 240.0f);
        size.y = std::max(size.y, 220.0f);

        const auto cursor = ImGui::GetCursorScreenPos();
        const auto min = cursor;
        const auto max = ImVec2(cursor.x + size.x, cursor.y + size.y);
        auto *drawList = ImGui::GetWindowDrawList();

        drawList->AddRectFilled(min, max, ImGui::GetColorU32(ImGuiCol_FrameBg), 8.0f);
        drawList->AddRect(min, max, ImGui::GetColorU32(ImGuiCol_Border), 8.0f, 0, 1.0f);

        const ImVec2 pad(42.0f, 26.0f);
        const ImVec2 phaseMin(min.x + pad.x, min.y + pad.y);
        const ImVec2 phaseMax(max.x - 22.0f, max.y - 34.0f);
        drawList->AddRect(phaseMin, phaseMax, ImGui::GetColorU32(ImGuiCol_Border), 0.0f, 0, 1.0f);

        const float criticalTemperature = 2.269185314f;
        const float xCritical = phaseMin.x + (criticalTemperature / 5.0f) * (phaseMax.x - phaseMin.x);
        drawList->AddLine(
            ImVec2(xCritical, phaseMin.y),
            ImVec2(xCritical, phaseMax.y),
            IM_COL32(196, 112, 86, 180),
            1.0f);

        const auto pointX = phaseMin.x + static_cast<float>(std::clamp(app.Ising.Temperature / 5.0, 0.0, 1.0)) * (phaseMax.x - phaseMin.x);
        const auto pointY = phaseMin.y + static_cast<float>((1.0 - std::clamp((app.Ising.Magnetization + 1.0) * 0.5, 0.0, 1.0))) * (phaseMax.y - phaseMin.y);
        drawList->AddCircleFilled(ImVec2(pointX, pointY), 5.5f, IM_COL32(245, 201, 96, 255));

        drawList->AddText(ImVec2(phaseMin.x, min.y + 4.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), "M = +1");
        drawList->AddText(ImVec2(phaseMin.x, phaseMax.y + 8.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), "M = -1");
        drawList->AddText(ImVec2(phaseMax.x - 46.0f, phaseMax.y + 8.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), "T = 5");
        drawList->AddText(ImVec2(xCritical + 4.0f, phaseMin.y + 4.0f), IM_COL32(216, 144, 116, 220), "Tc");

        ImGui::InvisibleButton(canvasId, size);
        ImGui::TextDisabled("Current point: T = %.3f, m = %.3f", app.Ising.Temperature, app.Ising.Magnetization);
    }

    auto DrawHysteresisTraceSurface(FWorkspaceSandboxApp &app,
                                    const char *canvasId,
                                    const char *caption) -> void {
        ImGui::TextDisabled("%s", caption);
        ImGui::Separator();

        ImVec2 size = ImGui::GetContentRegionAvail();
        size.x = std::max(size.x, 240.0f);
        size.y = std::max(size.y, 220.0f);

        const auto cursor = ImGui::GetCursorScreenPos();
        const auto min = cursor;
        const auto max = ImVec2(cursor.x + size.x, cursor.y + size.y);
        auto *drawList = ImGui::GetWindowDrawList();

        drawList->AddRectFilled(min, max, ImGui::GetColorU32(ImGuiCol_FrameBg), 8.0f);
        drawList->AddRect(min, max, ImGui::GetColorU32(ImGuiCol_Border), 8.0f, 0, 1.0f);

        const ImVec2 pad(42.0f, 26.0f);
        const ImVec2 plotMin(min.x + pad.x, min.y + pad.y);
        const ImVec2 plotMax(max.x - 22.0f, max.y - 30.0f);
        drawList->AddRect(plotMin, plotMax, ImGui::GetColorU32(ImGuiCol_Border), 0.0f, 0, 1.0f);

        const float yMid = plotMin.y + (plotMax.y - plotMin.y) * 0.5f;
        const float xMid = plotMin.x + (plotMax.x - plotMin.x) * 0.5f;
        drawList->AddLine(ImVec2(plotMin.x, yMid), ImVec2(plotMax.x, yMid), IM_COL32(120, 128, 136, 80), 1.0f);
        drawList->AddLine(ImVec2(xMid, plotMin.y), ImVec2(xMid, plotMax.y), IM_COL32(120, 128, 136, 80), 1.0f);

        if (app.Ising.ExternalFieldHistory.size() > 1 && app.Ising.MagnetizationFieldHistory.size() == app.Ising.ExternalFieldHistory.size()) {
            std::vector<ImVec2> hysteresisPoints;
            hysteresisPoints.reserve(app.Ising.ExternalFieldHistory.size());
            for (std::size_t i = 0; i < app.Ising.ExternalFieldHistory.size(); ++i) {
                const float hNorm = std::clamp((app.Ising.ExternalFieldHistory[i] + 1.0f) * 0.5f, 0.0f, 1.0f);
                const float mNorm = std::clamp((app.Ising.MagnetizationFieldHistory[i] + 1.0f) * 0.5f, 0.0f, 1.0f);
                const float x = plotMin.x + hNorm * (plotMax.x - plotMin.x);
                const float y = plotMax.y - mNorm * (plotMax.y - plotMin.y);
                hysteresisPoints.emplace_back(x, y);
            }

            drawList->AddPolyline(
                hysteresisPoints.data(),
                static_cast<int>(hysteresisPoints.size()),
                IM_COL32(120, 212, 255, 230),
                ImDrawFlags_None,
                2.0f);
            drawList->AddCircleFilled(hysteresisPoints.back(), 4.0f, IM_COL32(245, 201, 96, 255));
        } else {
            drawList->AddText(
                ImVec2(plotMin.x + 10.0f, plotMin.y + 12.0f),
                ImGui::GetColorU32(ImGuiCol_TextDisabled),
                "Run with varying external field to accumulate a loop.");
        }

        drawList->AddText(ImVec2(plotMin.x, plotMax.y + 6.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), "h = -1");
        drawList->AddText(ImVec2(plotMax.x - 34.0f, plotMax.y + 6.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), "h = +1");
        drawList->AddText(ImVec2(plotMin.x, plotMin.y - 2.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), "m = +1");
        drawList->AddText(ImVec2(plotMin.x, plotMax.y - 14.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), "m = -1");

        ImGui::InvisibleButton(canvasId, size);
        ImGui::TextDisabled("Current point: h = %.3f, m = %.3f", app.Ising.ExternalField, app.Ising.Magnetization);
    }
#endif

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

#if defined(STUDIOSLAB_WASM_ISING_WORKSPACE)
        addItem("ising_controls", WorkspaceIdSimulation, "Controls", &app.bShowIsingControls);
        addItem("spin_lattice", WorkspaceIdSimulation, "Lattice", &app.bShowSpinLattice);
        addItem("run_inspector", WorkspaceIdSimulation, "Inspector", &app.bShowRunInspector);
        addItem("phase_snapshot", WorkspaceIdSimulation, "Phase", &app.bShowPhaseSnapshot);
        addItem("hysteresis_trace", WorkspaceIdSimulation, "Loop", &app.bShowHysteresisTrace);
        addItem("event_console", WorkspaceIdSimulation, "Console", &app.bShowEventConsole);

        addItem("sampling_controls", WorkspaceIdObservables, "Controls", &app.bShowSamplingControls);
        addItem("observable_history", WorkspaceIdSimulation, "History", &app.bShowObservableHistory);
        addItem("sweep_statistics", WorkspaceIdObservables, "Stats", &app.bShowSweepStatistics);

        addItem("preset_browser", WorkspaceIdStudy, "Presets", &app.bShowPresetBrowser);
        addItem("model_notes", WorkspaceIdStudy, "Notes", &app.bShowModelNotes);
#else
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
#endif

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

#if defined(STUDIOSLAB_WASM_ISING_WORKSPACE)
        if (workspaceId == WorkspaceIdSimulation) {
            layout.Splits = {
                FDockNodeSplitV2{"main", "dock_left", "main", EDockSplitDirectionV2::Left, 0.24f},
                FDockNodeSplitV2{"main", "dock_right_outer", "main", EDockSplitDirectionV2::Right, 0.28f},
                FDockNodeSplitV2{"main", "dock_bottom", "main", EDockSplitDirectionV2::Down, 0.44f},
                FDockNodeSplitV2{"main", "dock_right_inner", "main", EDockSplitDirectionV2::Right, 0.50f}
            };
            layout.Placements = {
                FDockWindowPlacementV2{WindowTitleIsingControls, "dock_left"},
                FDockWindowPlacementV2{WindowTitleSpinLattice, "main"},
                FDockWindowPlacementV2{WindowTitleHysteresisTrace, "dock_right_inner"},
                FDockWindowPlacementV2{WindowTitlePhaseSnapshot, "dock_right_outer"},
                FDockWindowPlacementV2{WindowTitleRunInspector, "dock_bottom"},
                FDockWindowPlacementV2{WindowTitleObservableHistory, "dock_bottom", true},
                FDockWindowPlacementV2{WindowTitleEventConsole, "dock_bottom"}
            };
            return layout;
        }

        if (workspaceId == WorkspaceIdObservables) {
            layout.Splits = {
                FDockNodeSplitV2{"main", "dock_left", "main", EDockSplitDirectionV2::Left, 0.24f}
            };
            layout.Placements = {
                FDockWindowPlacementV2{WindowTitleSamplingControls, "dock_left"},
                FDockWindowPlacementV2{WindowTitleSweepStatistics, "main"}
            };
            return layout;
        }

        layout.Splits = {
            FDockNodeSplitV2{"main", "dock_left", "main", EDockSplitDirectionV2::Left, 0.24f}
        };
        layout.Placements = {
            FDockWindowPlacementV2{WindowTitlePresetBrowser, "dock_left"},
            FDockWindowPlacementV2{WindowTitleModelNotes, "main"}
        };
        return layout;
#else
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
#endif
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

#if defined(STUDIOSLAB_WASM_ISING_WORKSPACE)
        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "ising_controls",
            WindowTitleIsingControls,
            WorkspaceIdSimulation,
            &app.bShowIsingControls,
            false,
            false,
            [&app]() {
                auto &ising = app.Ising;
                ImGui::TextDisabled("Browser-local Metropolis kernel");
                ImGui::Separator();

                int latticeSize = ising.L;
                if (ImGui::SliderInt("Lattice L", &latticeSize, 16, 127)) {
                    if (latticeSize != ising.L) {
                        ising.L = latticeSize;
                        ising.bPendingReset = true;
                    }
                }

                float temperature = static_cast<float>(ising.Temperature);
                if (ImGui::SliderFloat("Temperature", &temperature, 0.10f, 5.00f, "%.3f")) {
                    ising.Temperature = temperature;
                }

                float externalField = static_cast<float>(ising.ExternalField);
                if (ImGui::SliderFloat("h field", &externalField, -1.00f, 1.00f, "%.3f")) {
                    ising.ExternalField = externalField;
                }

                int seedValue = static_cast<int>(ising.Seed);
                if (ImGui::InputInt("Seed", &seedValue)) {
                    ising.Seed = static_cast<unsigned int>(std::max(seedValue, 1));
                    ising.bPendingReset = true;
                }

                ImGui::Separator();

                int sweepsPerFrame = ising.SweepsPerFrame;
                if (ImGui::SliderInt("Sweeps / frame", &sweepsPerFrame, 1, 16)) {
                    ising.SweepsPerFrame = sweepsPerFrame;
                }

                bool bFerromagneticInitial = ising.bFerromagneticInitial;
                if (ImGui::Checkbox("Ferromagnetic init", &bFerromagneticInitial)) {
                    ising.bFerromagneticInitial = bFerromagneticInitial;
                    ising.bPendingReset = true;
                }

                if (ising.bPendingReset) {
                    ImGui::TextColored(ImVec4(0.93f, 0.72f, 0.35f, 1.0f), "Reset required to apply lattice changes.");
                } else {
                    ImGui::TextDisabled("Periodic boundaries, J = 1, one sweep = L^2 proposals.");
                }

                if (ImGui::Button(ising.bRunning ? "Pause" : "Run")) {
                    ising.bRunning = !ising.bRunning;
                    AppendEvent(app, ising.bRunning ? "Resumed browser Ising run" : "Paused browser Ising run");
                }
                ImGui::SameLine();
                if (ImGui::Button("Reset lattice")) {
                    ResetIsingState(app);
                }
                ImGui::SameLine();
                if (ImGui::Button("Single sweep")) {
                    if (ising.Spins.empty() || ising.bPendingReset) ResetIsingState(app, false);
                    RunIsingSweep(ising);
                }

                if (ImGui::Button("Advance 50 sweeps")) {
                    if (ising.Spins.empty() || ising.bPendingReset) ResetIsingState(app, false);
                    for (int i = 0; i < 50; ++i) RunIsingSweep(ising);
                    AppendEvent(app, "Advanced Ising run by 50 sweeps");
                }
                ImGui::SameLine();
                if (ImGui::Button("Jitter seed")) {
                    ++ising.Seed;
                    ising.bPendingReset = true;
                }

                ImGui::Separator();

                int historyPointBudget = ising.HistoryPointBudget;
                if (ImGui::SliderInt("History points", &historyPointBudget, 32, 960)) {
                    ising.HistoryPointBudget = historyPointBudget;
                    TrimAllIsingHistories(ising);
                }
                ImGui::TextDisabled("Applies to Observable History and the h x m loop trace.");

                ImGui::Separator();
                ImGui::TextDisabled("Browser presets");
                DrawIsingPresetControls(app);

                ImGui::Spacing();
                ImGui::TextDisabled("Critical temperature Tc ~= 2.269185");
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "spin_lattice",
            WindowTitleSpinLattice,
            WorkspaceIdSimulation,
            &app.bShowSpinLattice,
            false,
            false,
            [&app]() {
                DrawIsingLatticeSurface(app, "##IsingSpinLattice", "Ising spin field");
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "run_inspector",
            WindowTitleRunInspector,
            WorkspaceIdSimulation,
            &app.bShowRunInspector,
            false,
            false,
            [&app]() {
                const auto &ising = app.Ising;
                if (ImGui::BeginTable("##IsingRunInspector", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                    const auto addRow = [](const char *label, const Slab::Str &value) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::TextUnformatted(label);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextUnformatted(value.c_str());
                    };

                    addRow("Run state", ising.bRunning ? "running" : "paused");
                    addRow("Sweeps", std::to_string(ising.SweepCount));
                    addRow("Energy / spin", std::to_string(ising.EnergyDensity));
                    addRow("Magnetization", std::to_string(ising.Magnetization));
                    addRow("Acceptance", std::to_string(ising.AcceptanceRatio));
                    ImGui::EndTable();
                }

                ImGui::Spacing();
                ImGui::ProgressBar(
                    static_cast<float>(std::clamp((ising.Magnetization + 1.0) * 0.5, 0.0, 1.0)),
                    ImVec2(-1.0f, 0.0f),
                    "spin-up fraction");
                ImGui::ProgressBar(
                    static_cast<float>(std::clamp(ising.AcceptanceRatio, 0.0, 1.0)),
                    ImVec2(-1.0f, 0.0f),
                    "acceptance");

                ImGui::Spacing();
                if (ising.HoveredI >= 0 && ising.HoveredJ >= 0) {
                    const auto spin = IsingSpinAt(ising, ising.HoveredI, ising.HoveredJ);
                    const auto neighborSum = ComputeIsingNeighborSum(ising, ising.HoveredI, ising.HoveredJ);
                    const auto deltaEnergy = 2.0 * static_cast<double>(spin) * (static_cast<double>(neighborSum) + ising.ExternalField);
                    ImGui::Text("Hovered cell");
                    ImGui::Separator();
                    ImGui::Text("i, j: %d, %d", ising.HoveredI, ising.HoveredJ);
                    ImGui::Text("spin: %s", spin > 0 ? "+1" : "-1");
                    ImGui::Text("neighbor sum: %d", neighborSum);
                    ImGui::Text("delta E: %.3f", deltaEnergy);
                } else {
                    ImGui::TextDisabled("Hover the lattice to inspect a spin proposal.");
                }
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "event_console",
            WindowTitleEventConsole,
            WorkspaceIdSimulation,
            &app.bShowEventConsole,
            false,
            false,
            [&app]() {
                if (ImGui::Button("Clear log")) {
                    app.EventLog.clear();
                    AppendEvent(app, "Cleared event log");
                }
                ImGui::Separator();
                if (ImGui::BeginChild("##IsingEventLog")) {
                    for (const auto &line : app.EventLog) {
                        ImGui::TextUnformatted(line.c_str());
                    }
                }
                ImGui::EndChild();
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "sampling_controls",
            WindowTitleSamplingControls,
            WorkspaceIdObservables,
            &app.bShowSamplingControls,
            false,
            false,
            [&app]() {
                auto &ising = app.Ising;
                ImGui::TextDisabled("History sample count: %zu", ising.MagnetizationHistory.size());
                ImGui::Separator();
                if (ImGui::Button("Advance 200 sweeps")) {
                    if (ising.Spins.empty() || ising.bPendingReset) ResetIsingState(app, false);
                    for (int i = 0; i < 200; ++i) RunIsingSweep(ising);
                    AppendEvent(app, "Advanced Ising run by 200 sweeps");
                }
                if (ImGui::Button("Clear histories")) {
                    ising.MagnetizationHistory.clear();
                    ising.EnergyHistory.clear();
                    ising.AcceptanceHistory.clear();
                    ising.ExternalFieldHistory.clear();
                    ising.MagnetizationFieldHistory.clear();
                    MeasureIsingState(ising, ising.AcceptedLastSweep, ising.RejectedLastSweep);
                }
                ImGui::Spacing();
                ImGui::Checkbox("Animate preview grid", &app.bShowGrid);
                ImGui::TextDisabled("Live parameters update immediately. Lattice topology changes on reset.");
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "observable_history",
            WindowTitleObservableHistory,
            WorkspaceIdSimulation,
            &app.bShowObservableHistory,
            false,
            false,
            [&app]() {
                const auto &ising = app.Ising;
                if (ising.MagnetizationHistory.empty()) {
                    ImGui::TextDisabled("Run the simulation to populate observable history.");
                    return;
                }

                ImGui::TextDisabled("Magnetization");
                ImGui::PlotLines(
                    "##MagnetizationHistory",
                    ising.MagnetizationHistory.data(),
                    static_cast<int>(ising.MagnetizationHistory.size()),
                    0,
                    nullptr,
                    -1.05f,
                    1.05f,
                    ImVec2(-1.0f, 90.0f));

                ImGui::TextDisabled("Energy density");
                ImGui::PlotLines(
                    "##EnergyHistory",
                    ising.EnergyHistory.data(),
                    static_cast<int>(ising.EnergyHistory.size()),
                    0,
                    nullptr,
                    -2.5f,
                    0.5f,
                    ImVec2(-1.0f, 90.0f));

                ImGui::TextDisabled("Acceptance ratio");
                ImGui::PlotLines(
                    "##AcceptanceHistory",
                    ising.AcceptanceHistory.data(),
                    static_cast<int>(ising.AcceptanceHistory.size()),
                    0,
                    nullptr,
                    0.0f,
                    1.0f,
                    ImVec2(-1.0f, 90.0f));
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "sweep_statistics",
            WindowTitleSweepStatistics,
            WorkspaceIdObservables,
            &app.bShowSweepStatistics,
            false,
            false,
            [&app]() {
                const auto &ising = app.Ising;
                if (ImGui::BeginTable("##IsingSweepStats", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                    const auto addRow = [](const char *label, const Slab::Str &value) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::TextUnformatted(label);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextUnformatted(value.c_str());
                    };

                    addRow("Accepted last sweep", std::to_string(ising.AcceptedLastSweep));
                    addRow("Rejected last sweep", std::to_string(ising.RejectedLastSweep));
                    addRow("Sweeps / frame", std::to_string(ising.SweepsPerFrame));
                    addRow("Temperature", std::to_string(ising.Temperature));
                    addRow("External field", std::to_string(ising.ExternalField));
                    addRow("Seed", std::to_string(ising.Seed));
                    ImGui::EndTable();
                }
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "preset_browser",
            WindowTitlePresetBrowser,
            WorkspaceIdStudy,
            &app.bShowPresetBrowser,
            false,
            false,
            [&app]() {
                ImGui::TextDisabled("Browser presets");
                ImGui::Separator();
                DrawIsingPresetControls(app);
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "phase_snapshot",
            WindowTitlePhaseSnapshot,
            WorkspaceIdSimulation,
            &app.bShowPhaseSnapshot,
            false,
            false,
            [&app]() {
                DrawPhaseSnapshotSurface(app, "##IsingPhaseSnapshot", "Temperature / magnetization phase point");
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "hysteresis_trace",
            WindowTitleHysteresisTrace,
            WorkspaceIdSimulation,
            &app.bShowHysteresisTrace,
            false,
            false,
            [&app]() {
                DrawHysteresisTraceSurface(app, "##IsingHysteresisTrace", "External field x magnetization");
            }
        });

        panels.push_back(WindowingV2::FPanelSurfaceRegistrationV2{
            "model_notes",
            WindowTitleModelNotes,
            WorkspaceIdStudy,
            &app.bShowModelNotes,
            false,
            false,
            [&app]() {
                ImGui::Text("Ising Metropolis");
                ImGui::Separator();
                ImGui::BulletText("2D square lattice with periodic boundaries.");
                ImGui::BulletText("One sweep attempts L^2 random spin flips.");
                ImGui::BulletText("Acceptance uses exp(-deltaE / T) when deltaE > 0.");
                ImGui::BulletText("This wasm target keeps the model local to the browser sandbox.");
                ImGui::Spacing();
                ImGui::TextDisabled("Theme: %s", FindThemeDescriptor(app.ActiveThemeId).DisplayName);
            }
        });
#else
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
#endif

        return panels;
    }

    auto RenderFrame(void *userData) -> void {
        auto *app = static_cast<FWorkspaceSandboxApp *>(userData);
        if (app == nullptr || app->Window == nullptr) return;

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

#if defined(STUDIOSLAB_WASM_ISING_WORKSPACE)
        TickIsingSimulation(*app);
#endif

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
#if defined(STUDIOSLAB_WASM_ISING_WORKSPACE)
    app.Window = glfwCreateWindow(1440, 900, "StudioSlab WASM Ising Workspace", nullptr, nullptr);
#else
    app.Window = glfwCreateWindow(1440, 900, "StudioSlab WASM Workspace Sandbox", nullptr, nullptr);
#endif
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
