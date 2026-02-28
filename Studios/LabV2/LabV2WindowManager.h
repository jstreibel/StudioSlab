#ifndef STUDIOSLAB_LAB_V2_WINDOW_MANAGER_H
#define STUDIOSLAB_LAB_V2_WINDOW_MANAGER_H

#include "Graphics/Modules/ImGui/ImGuiContext.h"
#include "Graphics/Window/WindowManager.h"

#include "Math/Data/V2/LiveDataHubV2.h"
#include "Math/Data/V2/LiveControlHubV2.h"

#include <functional>
#include <array>

class FLabV2WindowManager final : public Slab::Graphics::FWindowManager {
public:
    explicit FLabV2WindowManager();

    void AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &) override;
    void AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &, bool hidden) override;

    bool NotifyRender(const Slab::Graphics::FPlatformWindow &) override;
    bool NotifySystemWindowReshape(int w, int h) override;

    [[nodiscard]] auto GetImGuiContext() const -> Slab::TPointer<Slab::Graphics::FImGuiContext>;
    [[nodiscard]] auto GetLiveDataHub() const -> Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2>;
    [[nodiscard]] auto GetLiveControlHub() const -> Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2>;

private:
    enum class EWorkspaceTab : unsigned char {
        Simulations = 0,
        Monitor,
        Schemes
    };

    struct FWorkspacePanelVisibility {
        bool bShowWindowLab = false;
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
    Slab::Str KG2DControlTopicPrefix = "labv2/control/kg2d";

    bool bUseDockspaceLayout = true;
    bool bResetDockLayoutRequested = false;
    bool bPendingViewRetile = true;
    unsigned int DockspaceId = 0;
    EWorkspaceTab ActiveWorkspace = EWorkspaceTab::Simulations;
    bool bWorkspaceLayoutsBootstrapped = false;
    std::array<bool, WorkspaceCount> WorkspaceLayoutInitialized = {false, false, false};
    std::array<FWorkspacePanelVisibility, WorkspaceCount> WorkspacePanels = {
        FWorkspacePanelVisibility{false, true, false, false, false, false, false},
        FWorkspacePanelVisibility{false, false, true, true, true, true, false},
        FWorkspacePanelVisibility{false, false, false, false, false, false, true}
    };
    float WorkspaceTabsHeight = 0.0f;
    bool bShowWindowLab = true;
    bool bShowWindowTasks = true;
    bool bShowWindowLiveData = true;
    bool bShowWindowLiveControl = true;
    bool bShowWindowViews = true;
    bool bShowWindowKG2DControl = true;
    bool bShowWindowBlueprints = false;

    auto FlushPendingSlabWindows() -> void;
    auto QueueSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) -> void;
    auto PruneClosedSlabWindows() -> bool;
    auto ArrangeTopLevelSlabWindows() -> void;
    auto DrawWorkspaceTabs() -> void;
    auto DrawDockspaceHost() -> void;
    auto DrawDockedToolWindows() -> void;
    auto DrawLegacySidePane() -> void;
    auto BuildDefaultDockLayout(unsigned int dockspaceId, EWorkspaceTab workspace) -> void;
    auto SaveWorkspacePanelVisibility(EWorkspaceTab workspace) -> void;
    auto LoadWorkspacePanelVisibility(EWorkspaceTab workspace) -> void;
    auto SetActiveWorkspace(EWorkspaceTab workspace) -> void;
    [[nodiscard]] auto IsDockingEnabled() const -> bool;
    auto DrawViewManagerPanel() -> void;
    auto FocusWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) -> void;
    [[nodiscard]] auto FindWindowByUniqueName(const Slab::Str &uniqueName) const
        -> Slab::TPointer<Slab::Graphics::FSlabWindow>;
};

#endif // STUDIOSLAB_LAB_V2_WINDOW_MANAGER_H
