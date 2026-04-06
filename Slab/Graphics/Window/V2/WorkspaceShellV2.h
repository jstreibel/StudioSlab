#ifndef STUDIOSLAB_WORKSPACE_SHELL_V2_H
#define STUDIOSLAB_WORKSPACE_SHELL_V2_H

#include "Graphics/Window/V2/WorkspaceLayoutV2.h"

#include <functional>
#include <map>
#include <span>

namespace Slab::Graphics::Windowing::V2 {

    struct FWorkspaceVisibilityItemV2 {
        Str ItemId;
        Str WorkspaceId;
        Str Label;
        bool *bVisible = nullptr;
        bool bDefaultVisible = false;
    };

    struct FPanelSurfaceRegistrationV2 {
        Str PanelId;
        Str WindowTitle;
        Str WorkspaceId;
        bool *bVisible = nullptr;
        bool bForceVisibleInWorkspace = false;
        bool bHideTitleBarWhenDocked = false;
        std::function<void()> DrawContents;
    };

    struct FPanelSurfaceHooksV2 {
        std::function<void(const FPanelSurfaceRegistrationV2 &)> BeforeBegin;
        std::function<void(const FPanelSurfaceRegistrationV2 &)> AfterDraw;
    };

    struct FWorkspaceShellStateV2 {
        bool bUseDockspaceLayout = true;
        bool bResetDockLayoutRequested = false;
        bool bWorkspaceLayoutsBootstrapped = false;
        float WorkspaceTabsHeight = 0.0f;
        float WorkspaceStripHeight = 0.0f;
        float WorkspaceLauncherWidth = 52.0f;
        unsigned int ActiveDockspaceId = 0;
        std::map<Str, bool> WorkspaceLayoutInitializedById;
        std::map<Str, std::map<Str, bool>> PanelVisibilityByWorkspaceId;
    };

    using FWorkspaceLauncherDrawV2 = std::function<void(const ImVec2 &)>;
    using FWorkspaceActivateCallbackV2 = std::function<void(const Str &)>;
    using FWorkspaceRetileCallbackV2 = std::function<void()>;
    using FWorkspaceDockLayoutBuilderV2 = std::function<FWorkspaceDockLayoutV2(const Str &)>;
    using FWorkspaceDockLayoutAppliedCallbackV2 = std::function<void(const Str &, const std::map<Str, ImGuiID> &)>;

    auto SaveWorkspaceVisibilityState(FWorkspaceShellStateV2 &state,
                                      std::span<const FWorkspaceVisibilityItemV2> items,
                                      const Str &workspaceId) -> void;
    auto LoadWorkspaceVisibilityState(const FWorkspaceShellStateV2 &state,
                                      std::span<const FWorkspaceVisibilityItemV2> items,
                                      const Str &workspaceId) -> void;

    auto DrawWorkspaceLauncher(FWorkspaceShellStateV2 &state,
                               float topMenuInset,
                               const FWorkspaceLauncherDrawV2 &drawLauncher,
                               const FWorkspaceRetileCallbackV2 &requestRetile,
                               const char *windowId = "##WorkspaceLauncher") -> void;
    auto DrawWorkspaceTabs(FWorkspaceShellStateV2 &state,
                           std::span<const FWorkspaceDefinitionV2> workspaces,
                           const Str &activeWorkspaceId,
                           float topMenuInset,
                           const FWorkspaceActivateCallbackV2 &setActiveWorkspace,
                           const FWorkspaceRetileCallbackV2 &requestRetile,
                           const char *windowId = "##WorkspaceTabs") -> void;
    auto DrawWorkspaceStrip(FWorkspaceShellStateV2 &state,
                            const FWorkspaceDefinitionV2 &activeWorkspace,
                            std::span<const FWorkspaceVisibilityItemV2> items,
                            float topMenuInset,
                            const FWorkspaceRetileCallbackV2 &requestRetile,
                            const char *windowId = "##WorkspaceStrip") -> void;
    auto DrawDockspaceHost(FWorkspaceShellStateV2 &state,
                           std::span<const FWorkspaceDefinitionV2> workspaces,
                           const Str &activeWorkspaceId,
                           float topMenuInset,
                           const FWorkspaceDockLayoutBuilderV2 &buildLayout,
                           const FWorkspaceDockLayoutAppliedCallbackV2 &onLayoutApplied,
                           const FWorkspaceRetileCallbackV2 &requestRetile,
                           const char *hostWindowId = "##WorkspaceDockspaceHost",
                           const char *dockspacePrefix = "##WorkspaceDockspace-") -> void;
    auto DrawPanelSurface(const FPanelSurfaceRegistrationV2 &registration,
                          const Str &activeWorkspaceId,
                          bool bDockingEnabled,
                          const FPanelSurfaceHooksV2 &hooks = {}) -> void;
    auto DrawDockedToolWindows(std::span<const FPanelSurfaceRegistrationV2> registrations,
                               const Str &activeWorkspaceId,
                               bool bDockingEnabled,
                               const FPanelSurfaceHooksV2 &hooks = {}) -> void;

} // namespace Slab::Graphics::Windowing::V2

#endif // STUDIOSLAB_WORKSPACE_SHELL_V2_H
