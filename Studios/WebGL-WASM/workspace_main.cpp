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

    auto ErrorCallback(const int errorCode, const char *description) -> void {
        std::fprintf(stderr, "GLFW error %d: %s\n", errorCode, description);
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
                if (ImGui::Button("WS", buttonSize)) {
                    app->bShowImGuiDemo = !app->bShowImGuiDemo;
                    AppendEvent(*app, app->bShowImGuiDemo ? "Opened Dear ImGui demo" : "Closed Dear ImGui demo");
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

    ImGui::StyleColorsDark();
    auto &style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.TabRounding = 6.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 6.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.10f, 0.14f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.11f, 0.14f, 0.19f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.18f, 0.24f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.15f, 0.20f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.31f, 0.41f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.19f, 0.26f, 0.35f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.16f, 0.22f, 0.29f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.33f, 0.43f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.40f, 0.51f, 1.0f);

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
