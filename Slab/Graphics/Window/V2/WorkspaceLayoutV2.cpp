#include "WorkspaceLayoutV2.h"

#ifdef IMGUI_HAS_DOCK
#include "imgui_internal.h"
#endif

#include <algorithm>
#include <map>

namespace Slab::Graphics::Windowing::V2 {

    namespace {

        auto ToImGuiDir(const EDockSplitDirectionV2 direction) -> ImGuiDir {
            switch (direction) {
                case EDockSplitDirectionV2::Left:
                    return ImGuiDir_Left;
                case EDockSplitDirectionV2::Right:
                    return ImGuiDir_Right;
                case EDockSplitDirectionV2::Up:
                    return ImGuiDir_Up;
                case EDockSplitDirectionV2::Down:
                    return ImGuiDir_Down;
            }

            return ImGuiDir_Left;
        }

        auto GetWindowTabId(const Str &windowTitle) -> ImGuiID {
            const ImGuiID windowId = ImHashStr(windowTitle.c_str());
            return ImHashStr("#TAB", 4, windowId);
        }

    } // namespace

    auto ApplyDockLayoutToImGui(const ImGuiID dockspaceId,
                                const ImVec2 &dockSize,
                                const FWorkspaceDockLayoutV2 &layout,
                                std::map<Str, ImGuiID> *outDockNodes) -> bool {
#ifndef IMGUI_HAS_DOCK
        (void) dockspaceId;
        (void) dockSize;
        (void) layout;
        (void) outDockNodes;
        return false;
#else
        if (dockspaceId == 0) return false;
        if (dockSize.x <= 1.0f || dockSize.y <= 1.0f) return false;

        ImGui::DockBuilderRemoveNode(dockspaceId);
        ImGui::DockBuilderAddNode(
            dockspaceId,
            static_cast<ImGuiDockNodeFlags>(
                static_cast<int>(ImGuiDockNodeFlags_DockSpace) |
                static_cast<int>(ImGuiDockNodeFlags_PassthruCentralNode) |
                static_cast<int>(ImGuiDockNodeFlags_AutoHideTabBar)));
        ImGui::DockBuilderSetNodeSize(dockspaceId, dockSize);

        std::map<Str, ImGuiID> dockNodes;
        dockNodes["main"] = dockspaceId;

        for (const auto &split : layout.Splits) {
            if (split.NewNodeId.empty() || split.RemainingNodeId.empty()) return false;

            const auto sourceIt = dockNodes.find(split.SourceNodeId);
            if (sourceIt == dockNodes.end()) return false;

            ImGuiID newNode = 0;
            ImGuiID remainingNode = 0;
            ImGui::DockBuilderSplitNode(
                sourceIt->second,
                ToImGuiDir(split.Direction),
                std::clamp(split.Ratio, 0.05f, 0.95f),
                &newNode,
                &remainingNode);

            dockNodes[split.NewNodeId] = newNode;
            dockNodes[split.RemainingNodeId] = remainingNode;
        }

        if (outDockNodes != nullptr) {
            *outDockNodes = dockNodes;
        }

        for (const auto &placement : layout.Placements) {
            if (placement.WindowTitle.empty()) continue;

            const auto nodeIt = dockNodes.find(placement.NodeId);
            if (nodeIt == dockNodes.end()) return false;

            ImGui::DockBuilderDockWindow(placement.WindowTitle.c_str(), nodeIt->second);

            if (placement.bSelectByDefault) {
                if (ImGuiDockNode *node = ImGui::DockBuilderGetNode(nodeIt->second); node != nullptr) {
                    node->SelectedTabId = GetWindowTabId(placement.WindowTitle);
                }
            }
        }

        ImGui::DockBuilderFinish(dockspaceId);
        return true;
#endif
    }

} // namespace Slab::Graphics::Windowing::V2
