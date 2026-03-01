#ifndef STUDIOSLAB_LAB_V2_WINDOW_MANAGER_H
#define STUDIOSLAB_LAB_V2_WINDOW_MANAGER_H

#include "Graphics/Modules/ImGui/ImGuiContext.h"
#include "Graphics/Window/WindowManager.h"

#include "Math/Data/V2/LiveDataHubV2.h"
#include "Math/Data/V2/LiveControlHubV2.h"
#include "Studios/Common/Simulations/V2/KGR2toRControlTopicsV2.h"

#include <functional>
#include <array>
#include <vector>

class FLabV2WindowManager final : public Slab::Graphics::FWindowManager {
public:
    explicit FLabV2WindowManager();

    void AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &) override;
    void AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &, bool hidden) override;

    bool NotifyRender(const Slab::Graphics::FPlatformWindow &) override;
    bool NotifySystemWindowReshape(int w, int h) override;
    bool NotifyKeyboard(Slab::Graphics::EKeyMap key,
                        Slab::Graphics::EKeyState state,
                        Slab::Graphics::EModKeys modKeys) override;
    bool NotifyCharacter(Slab::UInt codepoint) override;
    bool NotifyMouseButton(Slab::Graphics::EMouseButton button,
                           Slab::Graphics::EKeyState state,
                           Slab::Graphics::EModKeys modKeys) override;
    bool NotifyMouseMotion(int x, int y, int dx, int dy) override;
    bool NotifyMouseWheel(double dx, double dy) override;

    [[nodiscard]] auto GetImGuiContext() const -> Slab::TPointer<Slab::Graphics::FImGuiContext>;
    [[nodiscard]] auto GetLiveDataHub() const -> Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2>;
    [[nodiscard]] auto GetLiveControlHub() const -> Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2>;

private:
    enum class EWorkspaceTab : unsigned char {
        Simulations = 0,
        Monitor,
        Schemes
    };

    struct FPanelSurfaceRegistration {
        const char *WindowTitle = nullptr;
        EWorkspaceTab Workspace = EWorkspaceTab::Simulations;
        bool *bVisible = nullptr;
        bool bForceVisibleInWorkspace = false;
        bool bHideTitleBarWhenDocked = false;
        std::function<void()> DrawContents;
    };

    struct FWorkspacePanelVisibility {
        bool bShowWindowLab = false;
        bool bShowWindowSimulationLauncher = true;
        bool bShowWindowTasks = true;
        bool bShowWindowLiveData = true;
        bool bShowWindowLiveControl = true;
        bool bShowWindowViews = true;
        bool bShowWindowKG2DControl = true;
        bool bShowWindowBlueprints = false;
    };

    static constexpr std::size_t WorkspaceCount = 3;
    using FSlabWindowPtr = Slab::TPointer<Slab::Graphics::FSlabWindow>;
    using FSlabWindowVec = Slab::Vector<FSlabWindowPtr>;

    FSlabWindowVec SlabWindows;
    FSlabWindowVec PendingSlabWindows;

    int WidthSysWin = 200;
    int HeightSysWin = 200;
    int SidePaneWidth = 0;

    Slab::TPointer<Slab::Graphics::FImGuiContext> ImGuiContext;
    Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2> LiveDataHub;
    Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2> LiveControlHub;
    Slab::TPointer<class FSimulationManagerV2> SimulationManager;

    Slab::Str LiveDataTopicFilter;
    bool bLiveDataOnlyBound = false;
    Slab::Str SelectedLiveDataTopic;

    Slab::Str LiveControlTopicFilter;
    bool bLiveControlLevelsOnly = false;
    Slab::Str SelectedLiveControlTopic;

    Slab::Str SelectedViewUniqueName;

    Slab::Str TaskNameFilter;
    bool bTaskOnlyRunning = false;
    bool bTaskHideSuccess = false;
    bool bTaskOnlyNumeric = false;

    bool bPublishKG2DControlSource = true;
    Slab::DevFloat KG2DControlX = 0.0;
    Slab::DevFloat KG2DControlY = 0.0;
    Slab::DevFloat KG2DControlWidth = 0.35;
    Slab::DevFloat KG2DControlAmplitude = 0.0;
    bool bKG2DControlEnabled = false;
    Slab::Str KG2DControlTopicPrefix = Slab::Studios::Common::Simulations::V2::KG2DControlTopicPrefixDefaultV2;

    bool bUseDockspaceLayout = true;
    bool bResetDockLayoutRequested = false;
    bool bPendingViewRetile = true;
    int RetileStabilizationFramesRemaining = 0;
    bool bRequestLauncherInitialDock = false;
    unsigned int LauncherInitialDockId = 0;
    unsigned int DockspaceId = 0;
    EWorkspaceTab ActiveWorkspace = EWorkspaceTab::Simulations;
    bool bWorkspaceLayoutsBootstrapped = false;
    std::array<bool, WorkspaceCount> WorkspaceLayoutInitialized = {false, false, false};
    std::array<FWorkspacePanelVisibility, WorkspaceCount> WorkspacePanels = {
        FWorkspacePanelVisibility{false, true, true, false, false, false, false, false},
        FWorkspacePanelVisibility{false, false, false, true, true, true, true, false},
        FWorkspacePanelVisibility{false, false, false, false, false, false, false, true}
    };
    float WorkspaceTabsHeight = 0.0f;
    float WorkspaceStripHeight = 0.0f;
    float WorkspaceLauncherWidth = 52.0f;
    bool bShowWindowLab = true;
    bool bShowWindowSimulationLauncher = true;
    bool bShowWindowTasks = true;
    bool bShowWindowLiveData = true;
    bool bShowWindowLiveControl = true;
    bool bShowWindowViews = true;
    bool bShowWindowKG2DControl = true;
    bool bShowWindowBlueprints = false;
    bool bHasLastMousePosition = false;
    int LastMouseX = 0;
    int LastMouseY = 0;
    Slab::TVolatile<Slab::Graphics::FSlabWindow> CapturedMouseWindow;

    auto FlushPendingSlabWindows() -> void;
    auto QueueSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) -> void;
    auto PruneClosedSlabWindows() -> bool;
    auto ArrangeTopLevelSlabWindows() -> bool;
    auto DrawWorkspaceLauncher() -> void;
    auto DrawWorkspaceTabs() -> void;
    auto DrawWorkspaceStrip() -> void;
    auto DrawDockspaceHost() -> void;
    auto DrawDockedToolWindows() -> void;
    auto BuildPanelSurfaceRegistry() -> std::vector<FPanelSurfaceRegistration>;
    auto DrawPanelSurface(const FPanelSurfaceRegistration &registration) -> void;
    auto DrawLegacySidePane() -> void;
    auto BuildDefaultDockLayout(unsigned int dockspaceId, EWorkspaceTab workspace) -> void;
    [[nodiscard]] auto GetTopMenuInset() const -> float;
    auto RequestSimulationLauncherVisible() -> void;
    auto SaveWorkspacePanelVisibility(EWorkspaceTab workspace) -> void;
    auto LoadWorkspacePanelVisibility(EWorkspaceTab workspace) -> void;
    auto SetActiveWorkspace(EWorkspaceTab workspace) -> void;
    [[nodiscard]] auto IsDockingEnabled() const -> bool;
    [[nodiscard]] auto ShouldRenderSlabWindowsInWorkspace() const -> bool;
    auto HideSlabWindowsOffscreen() -> void;
    auto DrawViewManagerPanel() -> void;
    auto FocusWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) -> void;
    auto RequestViewRetile(int stabilizationFrames = 3) -> void;
    [[nodiscard]] auto FindTopWindowAtPoint(int x, int y) const -> Slab::TPointer<Slab::Graphics::FSlabWindow>;
    [[nodiscard]] auto FindKeyboardTargetWindow() const -> Slab::TPointer<Slab::Graphics::FSlabWindow>;
    auto SyncMousePositionFromImGui() -> void;
    [[nodiscard]] auto FindWindowByUniqueName(const Slab::Str &uniqueName) const
        -> Slab::TPointer<Slab::Graphics::FSlabWindow>;
};

#endif // STUDIOSLAB_LAB_V2_WINDOW_MANAGER_H
