#ifndef STUDIOSLAB_LAB_V2_WINDOW_MANAGER_H
#define STUDIOSLAB_LAB_V2_WINDOW_MANAGER_H

#include "Graphics/Modules/ImGui/ImGuiContext.h"
#include "Graphics/Window/WindowManager.h"

#include "Math/Data/V2/LiveDataHubV2.h"
#include "Math/Data/V2/LiveControlHubV2.h"
#include "Core/Reflection/V2/LegacyInterfaceAdapterV2.h"
#include "Core/Reflection/V2/ReflectionCatalogRegistryV2.h"
#include "Core/Reflection/V2/GraphSubstrateV2.h"
#include "Core/Reflection/V2/SemanticTypesV1.h"
#include "Graphics/Plot2D/V2/PlotReflectionCatalogV2.h"
#include "Graphics/Plot2D/V2/Plot2DWindowV2.h"
#include "imgui.h"

#include <functional>
#include <array>
#include <deque>
#include <map>
#include <vector>

class FLabV2WindowManager final : public Slab::Graphics::FWindowManager {
public:
    explicit FLabV2WindowManager();
    ~FLabV2WindowManager() override;

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
        Schemes,
        GraphPlayground,
        Plots
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
        bool bShowWindowLiveInteraction = true;
        bool bShowWindowViews = true;
        bool bShowWindowSchemeInspector = false;
        bool bShowWindowBlueprintGraph = false;
        bool bShowWindowGraphPlayground = false;
        bool bShowWindowPlotInspector = false;
    };

    static constexpr std::size_t WorkspaceCount = 4;
    using FSlabWindowPtr = Slab::TPointer<Slab::Graphics::FSlabWindow>;
    using FSlabWindowVec = Slab::Vector<FSlabWindowPtr>;
    struct FPendingSlabWindow {
        FSlabWindowPtr Window = nullptr;
        EWorkspaceTab Workspace = EWorkspaceTab::Simulations;
    };

    FSlabWindowVec SlabWindows;
    Slab::Vector<FPendingSlabWindow> PendingSlabWindows;
    std::map<Slab::Str, EWorkspaceTab> SlabWindowWorkspaceByUniqueName;

    int WidthSysWin = 200;
    int HeightSysWin = 200;
    int SidePaneWidth = 0;

    Slab::TPointer<Slab::Graphics::FImGuiContext> ImGuiContext;
    Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2> LiveDataHub;
    Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2> LiveControlHub;
    Slab::TPointer<class FSimulationManagerV2> SimulationManager;
    Slab::Core::Reflection::V2::FLegacyReflectionCatalogAdapterV2 ReflectionAdapter;
    Slab::Graphics::Plot2D::V2::FPlotReflectionCatalogV2 PlotReflectionAdapter;
    Slab::Str LegacyCatalogSourceId;
    Slab::Str PlotCatalogSourceId;
    Slab::Vector<Slab::Graphics::Plot2D::V2::FPlot2DWindowV2_ptr> PlotWindowsV2;
    std::map<Slab::Str, FSlabWindowPtr> PlotWindowHostsByWindowId;
    std::size_t BlueprintPlotWindowCreateCount = 0;
    std::size_t BlueprintPlotArtistCreateCount = 0;

    Slab::Str LiveDataTopicFilter;
    bool bLiveDataOnlyBound = false;
    Slab::Str SelectedLiveDataTopic;

    Slab::Str LiveControlTopicFilter;
    bool bLiveControlLevelsOnly = false;
    Slab::Str SelectedLiveControlTopic;
    Slab::Str LiveInteractionTopicFilter;

    Slab::Str SelectedViewUniqueName;

    Slab::Str TaskNameFilter;
    bool bTaskOnlyRunning = false;
    bool bTaskHideSuccess = false;
    bool bTaskOnlyNumeric = false;
    Slab::Str SelectedSchemeInterfaceId;
    Slab::Str SelectedSchemeParameterId;
    Slab::Str SelectedSchemeOperationId;
    std::map<Slab::Str, Slab::Str> SchemeParameterDraftByKey;
    Slab::Str SchemesLastOperationSummary;
    Slab::Core::Reflection::V2::FValueMapV2 SchemesLastOperationOutput;
    Slab::Str SelectedPlotInterfaceId;
    Slab::Str SelectedPlotParameterId;
    std::map<Slab::Str, Slab::Str> PlotParameterDraftByKey;
    Slab::Str PlotsLastOperationSummary;
    Slab::Core::Reflection::V2::FValueMapV2 PlotsLastOperationOutput;
    struct FSchemeOperationTraceEntry {
        std::size_t SequenceId = 0;
        Slab::Str InterfaceId;
        Slab::Str OperationId;
        Slab::Str Summary;
        bool bOk = false;
        Slab::Str LatencyLabel;
    };
    std::deque<FSchemeOperationTraceEntry> SchemesOperationTrace;
    std::size_t SchemesOperationTraceSequence = 0;
    Slab::Core::Reflection::V2::FGraphDocumentV2 SchemesBlueprintDocument;
    std::map<Slab::Str, ImVec2> BlueprintNodePositionById;
    bool bShowBlueprintLegend = true;
    bool bShowBlueprintTuningWindow = false;
    float BlueprintNodeHeaderScale = 1.55f;
    float BlueprintNodeHeaderMinHeight = 36.0f;
    float BlueprintNodeBodyTopPadding = 8.0f;
    float BlueprintNodeBodyBottomPadding = 10.0f;
    float BlueprintNodeBadgeRowGap = 4.0f;
    float BlueprintNodeActionReserve = 14.0f;
    float BlueprintNodeBadgeTopOffset = 42.0f;
    float BlueprintNodeActionGap = 10.0f;
    float BlueprintLegendPadding = 14.0f;
    float BlueprintLegendRowGap = 6.0f;
    float BlueprintLegendItemGap = 8.0f;
    float BlueprintLegendChipExtraHeight = 12.0f;
    float BlueprintLegendMarkerInset = 4.0f;
    Slab::Str BlueprintPressedNodeKey;
    ImVec2 BlueprintPressMousePos = ImVec2(0.0f, 0.0f);
    ImVec2 BlueprintPressNodePos = ImVec2(0.0f, 0.0f);
    bool bBlueprintNodeDragging = false;
    Slab::Str BlueprintPressedActionNodeKey;
    int BlueprintPressedActionIndex = -1;
    Slab::Str BlueprintGraphFilterText;
    bool bBlueprintGraphShowParameters = true;
    bool bBlueprintGraphShowQueries = true;
    bool bBlueprintGraphShowCommands = true;
    int BlueprintGraphMutabilityFilter = 0;
    float BlueprintGraphTraceHeight = 140.0f;

    struct FTemplateGraphPendingCoercionSuggestion {
        bool bActive = false;
        Slab::Str FromNodeId;
        Slab::Str FromPortId;
        Slab::Str ToNodeId;
        Slab::Str ToPortId;
        Slab::StrVector SuggestedCoercionOperatorIds;
        Slab::Str Summary;
    };

    Slab::Core::Reflection::V2::FGraphDocumentV2 PlaygroundTemplateDocument;
    Slab::Vector<Slab::Str> PlaygroundTemplateSelectedNodeIds;
    std::size_t PlaygroundTemplateNodeCounter = 0;
    std::size_t PlaygroundTemplateEdgeCounter = 0;
    std::size_t PlaygroundTemplateCoercionCounter = 0;
    Slab::Str PlaygroundTemplateSelectedOperatorId;
    Slab::Str PlaygroundTemplateSelectedNodeId;
    Slab::Str PlaygroundTemplateSelectedEdgeId;
    Slab::Str PlaygroundTemplateConnectingNodeId;
    Slab::Str PlaygroundTemplateConnectingPortId;
    bool bPlaygroundTemplateConnectingFromOutput = false;
    Slab::Str PlaygroundTemplateStatus;
    FTemplateGraphPendingCoercionSuggestion PlaygroundTemplatePendingCoercion;
    bool bPlaygroundTemplateMarqueeSelecting = false;
    ImVec2 PlaygroundTemplateMarqueeStart = ImVec2(0.0f, 0.0f);
    ImVec2 PlaygroundTemplateMarqueeEnd = ImVec2(0.0f, 0.0f);

    Slab::Core::Reflection::V2::FGraphDocumentV2 PlaygroundRoutingDocument;
    std::size_t PlaygroundRoutingEdgeCounter = 0;
    Slab::Str PlaygroundRoutingSourceEndpoint;
    Slab::Str PlaygroundRoutingTargetEndpoint;
    Slab::Core::Reflection::V2::EGraphEdgeKindV2 PlaygroundRoutingEdgeKind =
        Slab::Core::Reflection::V2::EGraphEdgeKindV2::ValueFlow;
    Slab::Str PlaygroundRoutingStatus;
    Slab::Str PlaygroundRuntimeFilter;
    Slab::Str PlaygroundPersistenceFilePath = "Build/bin/labv2_graph_playground.json";
    Slab::Str PlaygroundPersistenceStatus;
    bool bPlaygroundAutosave = true;
    bool bPlaygroundStateLoaded = false;
    bool bPlaygroundDirty = false;
    double PlaygroundLastAutosaveTimestampSeconds = 0.0;
    double PlaygroundAutosaveIntervalSeconds = 1.25;

    bool bUseDockspaceLayout = true;
    bool bResetDockLayoutRequested = false;
    bool bPendingViewRetile = true;
    int RetileStabilizationFramesRemaining = 0;
    bool bRequestLauncherInitialDock = false;
    unsigned int LauncherInitialDockId = 0;
    unsigned int DockspaceId = 0;
    EWorkspaceTab ActiveWorkspace = EWorkspaceTab::Simulations;
    bool bWorkspaceLayoutsBootstrapped = false;
    std::array<bool, WorkspaceCount> WorkspaceLayoutInitialized = {false, false, false, false};
    std::array<FWorkspacePanelVisibility, WorkspaceCount> WorkspacePanels = {
        FWorkspacePanelVisibility{false, true, true, true, true, true, true, false, false, false, false},
        FWorkspacePanelVisibility{false, false, false, false, false, false, false, true, true, false, false},
        FWorkspacePanelVisibility{false, false, false, false, false, false, false, false, false, true, false},
        FWorkspacePanelVisibility{false, false, false, false, false, false, false, false, false, false, true}
    };
    float WorkspaceTabsHeight = 0.0f;
    float WorkspaceStripHeight = 0.0f;
    float WorkspaceLauncherWidth = 52.0f;
    bool bShowWindowLab = true;
    bool bShowWindowSimulationLauncher = true;
    bool bShowWindowTasks = true;
    bool bShowWindowLiveData = true;
    bool bShowWindowLiveControl = true;
    bool bShowWindowLiveInteraction = true;
    bool bShowWindowViews = true;
    bool bShowWindowSchemeInspector = true;
    bool bShowWindowBlueprintGraph = true;
    bool bShowWindowGraphPlayground = true;
    bool bShowWindowPlotInspector = true;
    bool bHasLastMousePosition = false;
    int LastMouseX = 0;
    int LastMouseY = 0;
    Slab::TVolatile<Slab::Graphics::FSlabWindow> CapturedMouseWindow;

    auto FlushPendingSlabWindows() -> void;
    auto QueueSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) -> void;
    auto QueueSlabWindowInWorkspace(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window,
                                    EWorkspaceTab workspace) -> void;
    auto AddSlabWindowInWorkspace(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window,
                                  EWorkspaceTab workspace,
                                  bool hidden = false) -> void;
    auto SyncPlotWorkspaceWindows() -> void;
    auto PruneClosedSlabWindows() -> bool;
    [[nodiscard]] auto GetWorkspaceForWindow(const FSlabWindowPtr &window) const -> EWorkspaceTab;
    [[nodiscard]] auto GetWorkspaceWindows(EWorkspaceTab workspace) const -> FSlabWindowVec;
    auto ArrangeTopLevelSlabWindows() -> bool;
    auto DrawWorkspaceLauncher() -> void;
    auto DrawWorkspaceTabs() -> void;
    auto DrawWorkspaceStrip() -> void;
    auto DrawDockspaceHost() -> void;
    auto DrawDockedToolWindows() -> void;
    auto BuildPanelSurfaceRegistry() -> std::vector<FPanelSurfaceRegistration>;
    auto DrawPanelSurface(const FPanelSurfaceRegistration &registration) -> void;
    auto DrawSchemesInspectorPanel() -> void;
    auto DrawSchemesBlueprintGraphPanel() -> void;
    auto DrawGraphPlaygroundPanel() -> void;
    auto MarkGraphPlaygroundDirty() -> void;
    auto SaveGraphPlaygroundStateToFile() -> bool;
    auto LoadGraphPlaygroundStateFromFile() -> bool;
    auto DrawPlotsInspectorPanel() -> void;
    auto DrawLegacySidePane() -> void;
    auto BuildDefaultDockLayout(unsigned int dockspaceId, EWorkspaceTab workspace) -> void;
    [[nodiscard]] auto BuildReflectionInvocationContext() const -> Slab::Core::Reflection::V2::FInvocationContextV2;
    [[nodiscard]] auto BuildSchemeParameterDraftKey(const Slab::Str &interfaceId, const Slab::Str &parameterId) const -> Slab::Str;
    [[nodiscard]] auto BuildPlotParameterDraftKey(const Slab::Str &interfaceId, const Slab::Str &parameterId) const -> Slab::Str;
    auto EnsureSchemeSelectionIsValid(const Slab::Core::Reflection::V2::FReflectionCatalogV2 &catalog) -> void;
    auto EnsurePlotSelectionIsValid() -> void;
    auto ApplySchemeOperationResult(const Slab::Str &interfaceId,
                                    const Slab::Str &operationId,
                                    const Slab::Core::Reflection::V2::FOperationResultV2 &result) -> void;
    auto ApplyPlotOperationResult(const Slab::Str &interfaceId,
                                  const Slab::Str &operationId,
                                  const Slab::Core::Reflection::V2::FOperationResultV2 &result) -> void;
    auto InvokeSelectedSchemeOperation(const Slab::Core::Reflection::V2::FInterfaceSchemaV2 &interfaceSchema,
                                       const Slab::Str &operationId,
                                       const Slab::Core::Reflection::V2::FInvocationContextV2 &context) -> void;
    auto InvokeSelectedPlotOperation(const Slab::Core::Reflection::V2::FInterfaceSchemaV2 &interfaceSchema,
                                     const Slab::Str &operationId,
                                     const Slab::Core::Reflection::V2::FInvocationContextV2 &context) -> void;
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
