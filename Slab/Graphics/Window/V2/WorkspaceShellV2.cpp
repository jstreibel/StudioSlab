#include "WorkspaceShellV2.h"

#ifdef IMGUI_HAS_DOCK
#include "imgui_internal.h"
#endif

#include <algorithm>
#include <cmath>

namespace Slab::Graphics::Windowing::V2 {

    namespace {

        auto MakeDockspaceName(const char *prefix, const Str &workspaceId) -> Str {
            return Str(prefix) + workspaceId;
        }

        auto FindWorkspace(std::span<const FWorkspaceDefinitionV2> workspaces,
                           const Str &workspaceId) -> const FWorkspaceDefinitionV2 * {
            const auto it = std::find_if(workspaces.begin(), workspaces.end(), [&](const auto &workspace) {
                return workspace.WorkspaceId == workspaceId;
            });
            if (it == workspaces.end()) return nullptr;
            return &(*it);
        }

    } // namespace

    auto SaveWorkspaceVisibilityState(FWorkspaceShellStateV2 &state,
                                      const std::span<const FWorkspaceVisibilityItemV2> items,
                                      const Str &workspaceId) -> void {
        if (workspaceId.empty()) return;

        auto &saved = state.PanelVisibilityByWorkspaceId[workspaceId];
        for (const auto &item : items) {
            if (item.WorkspaceId != workspaceId) continue;
            if (item.ItemId.empty() || item.bVisible == nullptr) continue;
            saved[item.ItemId] = *item.bVisible;
        }
    }

    auto LoadWorkspaceVisibilityState(const FWorkspaceShellStateV2 &state,
                                      const std::span<const FWorkspaceVisibilityItemV2> items,
                                      const Str &workspaceId) -> void {
        if (workspaceId.empty()) return;

        const auto savedWorkspaceIt = state.PanelVisibilityByWorkspaceId.find(workspaceId);
        const auto *saved = savedWorkspaceIt != state.PanelVisibilityByWorkspaceId.end()
            ? &savedWorkspaceIt->second
            : nullptr;

        for (const auto &item : items) {
            if (item.WorkspaceId != workspaceId) continue;
            if (item.bVisible == nullptr) continue;

            if (saved != nullptr) {
                const auto savedItemIt = saved->find(item.ItemId);
                if (savedItemIt != saved->end()) {
                    *item.bVisible = savedItemIt->second;
                    continue;
                }
            }

            *item.bVisible = item.bDefaultVisible;
        }
    }

    auto DrawWorkspaceLauncher(FWorkspaceShellStateV2 &state,
                               const float topMenuInset,
                               const FWorkspaceLauncherDrawV2 &drawLauncher,
                               const FWorkspaceRetileCallbackV2 &requestRetile,
                               const char *windowId) -> void {
        if (!drawLauncher) return;

        const auto *viewport = ImGui::GetMainViewport();
        if (viewport == nullptr) return;

        const auto tabsHeight = ImGui::GetFrameHeight() + 8.0f;
        const auto stripHeight = ImGui::GetFrameHeight() + 8.0f;
        const auto totalHeight = tabsHeight + stripHeight;
        const auto desiredWidth = std::max(44.0f, totalHeight);

        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + topMenuInset), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(desiredWidth, totalHeight), ImGuiCond_Always);
        ImGui::SetNextWindowViewport(viewport->ID);

        constexpr auto flags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));

        const auto previousWidth = state.WorkspaceLauncherWidth;
        if (ImGui::Begin(windowId, nullptr, flags)) {
            const auto buttonSize = ImVec2(
                std::max(16.0f, ImGui::GetContentRegionAvail().x),
                std::max(16.0f, ImGui::GetContentRegionAvail().y));
            drawLauncher(buttonSize);
            state.WorkspaceLauncherWidth = std::max(0.0f, ImGui::GetWindowWidth());
        }
        ImGui::End();

        ImGui::PopStyleVar(4);

        if (requestRetile && std::abs(previousWidth - state.WorkspaceLauncherWidth) > 0.5f) {
            requestRetile();
        }
    }

    auto DrawWorkspaceTabs(FWorkspaceShellStateV2 &state,
                           const std::span<const FWorkspaceDefinitionV2> workspaces,
                           const Str &activeWorkspaceId,
                           const float topMenuInset,
                           const FWorkspaceActivateCallbackV2 &setActiveWorkspace,
                           const FWorkspaceRetileCallbackV2 &requestRetile,
                           const char *windowId) -> void {
        const auto *viewport = ImGui::GetMainViewport();
        if (viewport == nullptr) return;

        const float estimatedHeight = ImGui::GetFrameHeight() + 8.0f;
        const auto leftInset = std::max(0.0f, state.WorkspaceLauncherWidth);

        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + leftInset, viewport->Pos.y + topMenuInset), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(std::max(0.0f, viewport->Size.x - leftInset), estimatedHeight), ImGuiCond_Always);
        ImGui::SetNextWindowViewport(viewport->ID);

        constexpr auto flags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 3.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));

        const auto previousHeight = state.WorkspaceTabsHeight;
        state.WorkspaceTabsHeight = estimatedHeight;
        if (ImGui::Begin(windowId, nullptr, flags)) {
            auto selectedWorkspaceId = activeWorkspaceId;
            if (ImGui::BeginTabBar("##WorkspaceTabBar",
                ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
                for (const auto &workspace : workspaces) {
                    if (ImGui::BeginTabItem(workspace.DisplayName.c_str())) {
                        selectedWorkspaceId = workspace.WorkspaceId;
                        ImGui::EndTabItem();
                    }
                }

                ImGui::EndTabBar();
            }

            if (setActiveWorkspace != nullptr && selectedWorkspaceId != activeWorkspaceId) {
                setActiveWorkspace(selectedWorkspaceId);
            }
        }
        ImGui::End();
        ImGui::PopStyleVar(4);

        if (requestRetile && std::abs(previousHeight - state.WorkspaceTabsHeight) > 0.5f) {
            requestRetile();
        }
    }

    auto DrawWorkspaceStrip(FWorkspaceShellStateV2 &state,
                            const FWorkspaceDefinitionV2 &activeWorkspace,
                            const std::span<const FWorkspaceVisibilityItemV2> items,
                            const float topMenuInset,
                            const FWorkspaceRetileCallbackV2 &requestRetile,
                            const char *windowId) -> void {
        const auto *viewport = ImGui::GetMainViewport();
        if (viewport == nullptr) return;

        const float estimatedHeight = ImGui::GetFrameHeight() + 8.0f;
        const auto yOffset = topMenuInset + state.WorkspaceTabsHeight;
        const auto leftInset = std::max(0.0f, state.WorkspaceLauncherWidth);

        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + leftInset, viewport->Pos.y + yOffset), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(std::max(0.0f, viewport->Size.x - leftInset), estimatedHeight), ImGuiCond_Always);
        ImGui::SetNextWindowViewport(viewport->ID);

        constexpr auto flags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 2.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));

        const auto previousHeight = state.WorkspaceStripHeight;
        state.WorkspaceStripHeight = estimatedHeight;
        if (ImGui::Begin(windowId, nullptr, flags)) {
            ImGui::TextDisabled("%s", activeWorkspace.DisplayName.c_str());
            ImGui::SameLine();
            ImGui::TextDisabled("|");
            ImGui::SameLine();

            for (const auto &item : items) {
                if (item.WorkspaceId != activeWorkspace.WorkspaceId) continue;
                if (item.Label.empty() || item.bVisible == nullptr) continue;

                ImGui::Checkbox(item.Label.c_str(), item.bVisible);
                ImGui::SameLine();
            }

            if (ImGui::Button("Reset Layout")) {
                state.bResetDockLayoutRequested = true;
            }
        }
        ImGui::End();

        ImGui::PopStyleVar(4);

        if (requestRetile && std::abs(previousHeight - state.WorkspaceStripHeight) > 0.5f) {
            requestRetile();
        }
    }

    auto DrawDockspaceHost(FWorkspaceShellStateV2 &state,
                           const std::span<const FWorkspaceDefinitionV2> workspaces,
                           const Str &activeWorkspaceId,
                           const float topMenuInset,
                           const FWorkspaceDockLayoutBuilderV2 &buildLayout,
                           const FWorkspaceDockLayoutAppliedCallbackV2 &onLayoutApplied,
                           const FWorkspaceRetileCallbackV2 &requestRetile,
                           const char *hostWindowId,
                           const char *dockspacePrefix) -> void {
        const auto *viewport = ImGui::GetMainViewport();
        if (viewport == nullptr) return;

        const auto topChromeHeight = topMenuInset + state.WorkspaceTabsHeight + state.WorkspaceStripHeight;
        ImGui::SetNextWindowPos(
            ImVec2(viewport->Pos.x, viewport->Pos.y + topChromeHeight),
            ImGuiCond_Always);
        ImGui::SetNextWindowSize(
            ImVec2(viewport->Size.x, std::max(0.0f, viewport->Size.y - topChromeHeight)),
            ImGuiCond_Always);
        ImGui::SetNextWindowViewport(viewport->ID);

        constexpr auto hostFlags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        if (ImGui::Begin(hostWindowId, nullptr, hostFlags)) {
            std::map<Str, unsigned int> dockspaceIdsByWorkspaceId;
            for (const auto &workspace : workspaces) {
                dockspaceIdsByWorkspaceId[workspace.WorkspaceId] =
                    static_cast<unsigned int>(ImGui::GetID(MakeDockspaceName(dockspacePrefix, workspace.WorkspaceId).c_str()));
            }

            const auto dockSize = ImVec2(
                viewport->Size.x,
                std::max(0.0f, viewport->Size.y - topChromeHeight));

            if (!state.bWorkspaceLayoutsBootstrapped && buildLayout != nullptr) {
                for (const auto &workspace : workspaces) {
                    const auto dockspaceIdIt = dockspaceIdsByWorkspaceId.find(workspace.WorkspaceId);
                    if (dockspaceIdIt == dockspaceIdsByWorkspaceId.end()) continue;

                    std::map<Str, ImGuiID> dockNodes;
                    if (!ApplyDockLayoutToImGui(
                            static_cast<ImGuiID>(dockspaceIdIt->second),
                            dockSize,
                            buildLayout(workspace.WorkspaceId),
                            &dockNodes)) {
                        continue;
                    }

                    state.WorkspaceLayoutInitializedById[workspace.WorkspaceId] = true;
                    if (onLayoutApplied) onLayoutApplied(workspace.WorkspaceId, dockNodes);
                }

                state.bWorkspaceLayoutsBootstrapped = true;
                if (requestRetile) requestRetile();
            }

            const auto activeDockspaceIdIt = dockspaceIdsByWorkspaceId.find(activeWorkspaceId);
            state.ActiveDockspaceId = activeDockspaceIdIt != dockspaceIdsByWorkspaceId.end()
                ? activeDockspaceIdIt->second
                : 0u;

            if (state.bResetDockLayoutRequested && buildLayout != nullptr && !activeWorkspaceId.empty()) {
                if (const auto resetDockspaceIdIt = dockspaceIdsByWorkspaceId.find(activeWorkspaceId);
                    resetDockspaceIdIt != dockspaceIdsByWorkspaceId.end()) {
                    std::map<Str, ImGuiID> dockNodes;
                    if (ApplyDockLayoutToImGui(
                            static_cast<ImGuiID>(resetDockspaceIdIt->second),
                            dockSize,
                            buildLayout(activeWorkspaceId),
                            &dockNodes)) {
                        state.WorkspaceLayoutInitializedById[activeWorkspaceId] = true;
                        if (onLayoutApplied) onLayoutApplied(activeWorkspaceId, dockNodes);
                    }
                }

                state.bResetDockLayoutRequested = false;
                if (requestRetile) requestRetile();
            }

#ifdef IMGUI_HAS_DOCK
            for (const auto &workspace : workspaces) {
                const auto dockspaceIdIt = dockspaceIdsByWorkspaceId.find(workspace.WorkspaceId);
                if (dockspaceIdIt == dockspaceIdsByWorkspaceId.end()) continue;

                ImGuiDockNodeFlags dockFlags = static_cast<ImGuiDockNodeFlags>(
                    static_cast<int>(ImGuiDockNodeFlags_PassthruCentralNode) |
                    static_cast<int>(ImGuiDockNodeFlags_AutoHideTabBar));
                if (workspace.WorkspaceId != activeWorkspaceId) {
                    dockFlags = static_cast<ImGuiDockNodeFlags>(
                        static_cast<int>(ImGuiDockNodeFlags_KeepAliveOnly) |
                        static_cast<int>(ImGuiDockNodeFlags_AutoHideTabBar));
                }

                ImGui::DockSpace(static_cast<ImGuiID>(dockspaceIdIt->second), ImVec2(0.0f, 0.0f), dockFlags);
            }
#endif
        }
        ImGui::End();

        ImGui::PopStyleVar(3);
    }

    auto DrawPanelSurface(const FPanelSurfaceRegistrationV2 &registration,
                          const Str &activeWorkspaceId,
                          const bool bDockingEnabled,
                          const FPanelSurfaceHooksV2 &hooks) -> void {
        if (registration.WorkspaceId != activeWorkspaceId) return;

        const bool bVisible = registration.bForceVisibleInWorkspace ||
            (registration.bVisible != nullptr && *registration.bVisible);
        if (!bVisible) return;

        if (hooks.BeforeBegin) hooks.BeforeBegin(registration);

        const auto openPtr = registration.bForceVisibleInWorkspace ? nullptr : registration.bVisible;
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
        if (bDockingEnabled && registration.bHideTitleBarWhenDocked) {
            windowFlags |= ImGuiWindowFlags_NoTitleBar;
        }

        if (ImGui::Begin(registration.WindowTitle.c_str(), openPtr, windowFlags)) {
            if (registration.DrawContents) registration.DrawContents();
            if (hooks.AfterDraw) hooks.AfterDraw(registration);
        }
        ImGui::End();
    }

    auto DrawDockedToolWindows(const std::span<const FPanelSurfaceRegistrationV2> registrations,
                               const Str &activeWorkspaceId,
                               const bool bDockingEnabled,
                               const FPanelSurfaceHooksV2 &hooks) -> void {
        for (const auto &registration : registrations) {
            DrawPanelSurface(registration, activeWorkspaceId, bDockingEnabled, hooks);
        }
    }

} // namespace Slab::Graphics::Windowing::V2
