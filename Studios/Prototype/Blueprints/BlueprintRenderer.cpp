//
// Created by joao on 11/2/24.
//

#include "BlueprintRenderer.h"

#include <utility>

#include "Utilities/drawing.h"
#include "Utilities/widgets.h"
#include "Core/Tools/Resources.h"
#include "StudioSlab.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Graphics/SlabGraphics.h"


namespace Slab::Blueprints {

    using namespace ax;

    using ax::Widgets::IconType;

    BlueprintRenderer::BlueprintRenderer(Pointer<Slab::Blueprints::Blueprint> blueprint, Graphics::ParentSystemWindow parent_syswin)
    : Graphics::FSystemWindowEventListener(parent_syswin)
    , blueprint(std::move(blueprint)) {
        // Blueprints "OnStart()"

        auto &module = Slab::GetModule<Graphics::ImGuiModule>("ImGui");
        m_Context = DynamicPointerCast<Graphics::SlabImGuiContext>(module.createContext(parent_system_window));
        addResponder(m_Context);

        ed::Config config;

        config.SettingsFile = "Blueprints.json";

        config.UserPointer = this;

        config.LoadNodeSettings = [](ed::NodeId nodeId, char* data, void* userPointer) -> size_t
        {
            auto self = static_cast<BlueprintRenderer*>(userPointer);

            auto node = self->blueprint->FindNode(nodeId);
            if (!node)
                return 0;

            if (data != nullptr)
                memcpy((void*)data, (void*)(node->State.data()), node->State.size());
            return node->State.size();
        };

        config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data,
                size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
        {
            auto self = static_cast<BlueprintRenderer*>(userPointer);

            auto node = self->blueprint->FindNode(nodeId);
            if (!node)
                return false;

            node->State.assign(data, size);

            self->TouchNode(nodeId);

            return true;
        };

        m_Editor = ed::CreateEditor(&config);
        ed::SetCurrentEditor(m_Editor);

        auto location = Slab::Core::Resources::Folder + "Blueprints/";
        m_HeaderBackground = Slab::Graphics::LoadTexture(location + "BlueprintBackground.png");
        m_SaveIcon         = Slab::Graphics::LoadTexture(location + "ic_save_white_24dp.png");
        m_RestoreIcon      = Slab::Graphics::LoadTexture(location + "ic_restore_white_24dp.png");
    }

    void BlueprintRenderer::TouchNode(ed::NodeId id) {
        m_NodeTouchTime[id] = m_TouchTime;
    }

    float BlueprintRenderer::GetTouchProgress(ed::NodeId id) {
        auto it = m_NodeTouchTime.find(id);
        if (it != m_NodeTouchTime.end() && it->second > 0.0f)
            return (m_TouchTime - it->second) / m_TouchTime;
        else
            return 0.0f;
    }

    void BlueprintRenderer::UpdateTouch() {
        const auto deltaTime = ImGui::GetIO().DeltaTime;
        for (auto& entry : m_NodeTouchTime)
        {
            if (entry.second > 0.0f)
                entry.second -= deltaTime;
        }
    }

    ImColor BlueprintRenderer::GetIconColor(PinType type) {
        switch (type)
        {
            default:
            case PinType::Flow:     return {255, 255, 255};
            case PinType::Bool:     return {220,  48,  48};
            case PinType::Int:      return { 68, 201, 156};
            case PinType::Float:    return {147, 226,  74};
            case PinType::String:   return {124,  21, 153};
            case PinType::Object:   return { 51, 150, 215};
            case PinType::Function: return {218,   0, 183};
            case PinType::Delegate: return {255,  48,  48};
        }
    }

    void BlueprintRenderer::DrawPinIcon(const Pin &pin, bool connected, int alpha) {
        IconType iconType;
        ImColor  color = GetIconColor(pin.Type);
        color.Value.w = alpha / 255.0f;
        switch (pin.Type)
        {
            case PinType::Flow:     iconType = IconType::Flow;   break;
            case PinType::Bool:     iconType = IconType::Circle; break;
            case PinType::Int:      iconType = IconType::Circle; break;
            case PinType::Float:    iconType = IconType::Circle; break;
            case PinType::String:   iconType = IconType::Circle; break;
            case PinType::Object:   iconType = IconType::Circle; break;
            case PinType::Function: iconType = IconType::Circle; break;
            case PinType::Delegate: iconType = IconType::Square; break;
            default:
                return;
        }

        ax::Widgets::Icon(ImVec2(static_cast<float>(m_PinIconSize), static_cast<float>(m_PinIconSize)), iconType, connected, color, ImColor(32, 32, 32, alpha));
    }

    void BlueprintRenderer::ShowStyleEditor(bool *show) {
        if (!ImGui::Begin("Style", show))
        {
            ImGui::End();
            return;
        }

        auto paneWidth = ImGui::GetContentRegionAvail().x;

        auto& editorStyle = ed::GetStyle();
        ImGui::BeginHorizontal("Style buttons", ImVec2(paneWidth, 0), 1.0f);
        ImGui::TextUnformatted("Values");
        ImGui::Spring();
        if (ImGui::Button("Reset to defaults"))
            editorStyle = ed::Style();
        ImGui::EndHorizontal();
        ImGui::Spacing();
        ImGui::DragFloat4("Node Padding", &editorStyle.NodePadding.x, 0.1f, 0.0f, 40.0f);
        ImGui::DragFloat("Node Rounding", &editorStyle.NodeRounding, 0.1f, 0.0f, 40.0f);
        ImGui::DragFloat("Node Border Width", &editorStyle.NodeBorderWidth, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat("Hovered Node Border Width", &editorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat("Hovered Node Border Offset", &editorStyle.HoverNodeBorderOffset, 0.1f, -40.0f, 40.0f);
        ImGui::DragFloat("Selected Node Border Width", &editorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat("Selected Node Border Offset", &editorStyle.SelectedNodeBorderOffset, 0.1f, -40.0f, 40.0f);
        ImGui::DragFloat("Pin Rounding", &editorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
        ImGui::DragFloat("Pin Border Width", &editorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat("Link Strength", &editorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
        //ImVec2  SourceDirection;
        //ImVec2  TargetDirection;
        ImGui::DragFloat("Scroll Duration", &editorStyle.ScrollDuration, 0.001f, 0.0f, 2.0f);
        ImGui::DragFloat("Flow Marker Distance", &editorStyle.FlowMarkerDistance, 1.0f, 1.0f, 200.0f);
        ImGui::DragFloat("Flow Speed", &editorStyle.FlowSpeed, 1.0f, 1.0f, 2000.0f);
        ImGui::DragFloat("Flow Duration", &editorStyle.FlowDuration, 0.001f, 0.0f, 5.0f);
        //ImVec2  PivotAlignment;
        //ImVec2  PivotSize;
        //ImVec2  PivotScale;
        //float   PinCorners;
        //float   PinRadius;
        //float   PinArrowSize;
        //float   PinArrowWidth;
        ImGui::DragFloat("Group Rounding", &editorStyle.GroupRounding, 0.1f, 0.0f, 40.0f);
        ImGui::DragFloat("Group Border Width", &editorStyle.GroupBorderWidth, 0.1f, 0.0f, 15.0f);

        ImGui::Separator();

        static ImGuiColorEditFlags edit_mode = ImGuiColorEditFlags_DisplayRGB;
        ImGui::BeginHorizontal("Color Mode", ImVec2(paneWidth, 0), 1.0f);
        ImGui::TextUnformatted("Filter Colors");
        ImGui::Spring();
        ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditFlags_DisplayRGB);
        ImGui::Spring(0);
        ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditFlags_DisplayHSV);
        ImGui::Spring(0);
        ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditFlags_DisplayHex);
        ImGui::EndHorizontal();

        static ImGuiTextFilter filter;
        filter.Draw("##filter", paneWidth);

        ImGui::Spacing();

        ImGui::PushItemWidth(-160);
        for (int i = 0; i < ed::StyleColor_Count; ++i)
        {
            auto name = ed::GetStyleColorName((ed::StyleColor)i);
            if (!filter.PassFilter(name))
                continue;

            ImGui::ColorEdit4(name, &editorStyle.Colors[i].x, edit_mode);
        }
        ImGui::PopItemWidth();

        ImGui::End();
    }

    void BlueprintRenderer::ShowLeftPane(float paneWidth) {
        auto& io = ImGui::GetIO();

        ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

        paneWidth = ImGui::GetContentRegionAvail().x;

        static bool showStyleEditor = false;
        ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
        ImGui::Spring(0.0f, 0.0f);
        if (ImGui::Button("Zoom to Content"))
            ed::NavigateToContent();
        ImGui::Spring(0.0f);
        if (ImGui::Button("Show Flow"))
        {
            if(blueprint != nullptr) {
                auto links = blueprint->GetLinks();

                for (auto &link: links)
                    ed::Flow(link.ID);
            }
        }
        ImGui::Spring();
        if (ImGui::Button("Edit Style"))
            showStyleEditor = true;
        ImGui::EndHorizontal();
        ImGui::Checkbox("Show Ordinals", &m_ShowOrdinals);

        if (showStyleEditor)
            ShowStyleEditor(&showStyleEditor);

        std::vector<ed::NodeId> selectedNodes;
        std::vector<ed::LinkId> selectedLinks;
        selectedNodes.resize(ed::GetSelectedObjectCount());
        selectedLinks.resize(ed::GetSelectedObjectCount());

        int nodeCount = ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
        int linkCount = ed::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));

        selectedNodes.resize(nodeCount);
        selectedLinks.resize(linkCount);

        int saveIconWidth     = Slab::Graphics::GetTextureWidth(m_SaveIcon);
        int saveIconHeight    = Slab::Graphics::GetTextureWidth(m_SaveIcon);
        int restoreIconWidth  = Slab::Graphics::GetTextureWidth(m_RestoreIcon);
        int restoreIconHeight = Slab::Graphics::GetTextureWidth(m_RestoreIcon);

        ImGui::GetWindowDrawList()->AddRectFilled(
                ImGui::GetCursorScreenPos(),
                ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
                ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
        ImGui::Spacing(); ImGui::SameLine();
        ImGui::TextUnformatted("Nodes");
        ImGui::Indent();
        if(blueprint != nullptr) {
            auto nodes = blueprint->GetNodes();

            for (auto &node: nodes) {
                ImGui::PushID(node.ID.AsPointer());
                auto start = ImGui::GetCursorScreenPos();

                if (const auto progress = GetTouchProgress(node.ID)) {
                    ImGui::GetWindowDrawList()->AddLine(
                            start + ImVec2(-8, 0),
                            start + ImVec2(-8, ImGui::GetTextLineHeight()),
                            IM_COL32(255, 0, 0, 255 - (int) (255 * progress)), 4.0f);
                }

                bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(), node.ID) != selectedNodes.end();
# if IMGUI_VERSION_NUM >= 18967
                ImGui::SetNextItemAllowOverlap();
# endif
                if (ImGui::Selectable(
                        (node.Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer()))).c_str(),
                        &isSelected)) {
                    if (io.KeyCtrl) {
                        if (isSelected)
                            ed::SelectNode(node.ID, true);
                        else
                            ed::DeselectNode(node.ID);
                    } else
                        ed::SelectNode(node.ID, false);

                    ed::NavigateToSelection();
                }
                if (ImGui::IsItemHovered() && !node.State.empty())
                    ImGui::SetTooltip("State: %s", node.State.c_str());

                auto id = std::string("(") + std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer())) + ")";
                auto textSize = ImGui::CalcTextSize(id.c_str(), nullptr);
                auto iconPanelPos = start + ImVec2(
                        paneWidth - ImGui::GetStyle().FramePadding.x - ImGui::GetStyle().IndentSpacing - saveIconWidth -
                        restoreIconWidth - ImGui::GetStyle().ItemInnerSpacing.x * 1,
                        (ImGui::GetTextLineHeight() - saveIconHeight) / 2);
                ImGui::GetWindowDrawList()->AddText(
                        ImVec2(iconPanelPos.x - textSize.x - ImGui::GetStyle().ItemInnerSpacing.x, start.y),
                        IM_COL32(255, 255, 255, 255), id.c_str(), nullptr);

                auto drawList = ImGui::GetWindowDrawList();
                ImGui::SetCursorScreenPos(iconPanelPos);
# if IMGUI_VERSION_NUM < 18967
                ImGui::SetItemAllowOverlap();
# else
                ImGui::SetNextItemAllowOverlap();
# endif
                if (node.SavedState.empty()) {
                    if (ImGui::InvisibleButton("save", ImVec2((float) saveIconWidth, (float) saveIconHeight)))
                        node.SavedState = node.State;

                    if (ImGui::IsItemActive())
                        drawList->AddImage(m_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0),
                                           ImVec2(1, 1), IM_COL32(255, 255, 255, 96));
                    else if (ImGui::IsItemHovered())
                        drawList->AddImage(m_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0),
                                           ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
                    else
                        drawList->AddImage(m_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0),
                                           ImVec2(1, 1), IM_COL32(255, 255, 255, 160));
                } else {
                    ImGui::Dummy(ImVec2((float) saveIconWidth, (float) saveIconHeight));
                    drawList->AddImage(m_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0),
                                       ImVec2(1, 1), IM_COL32(255, 255, 255, 32));
                }

                ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
# if IMGUI_VERSION_NUM < 18967
                ImGui::SetItemAllowOverlap();
# else
                ImGui::SetNextItemAllowOverlap();
# endif
                if (!node.SavedState.empty()) {
                    if (ImGui::InvisibleButton("restore",
                                               ImVec2((float) restoreIconWidth, (float) restoreIconHeight))) {
                        node.State = node.SavedState;
                        ed::RestoreNodeState(node.ID);
                        node.SavedState.clear();
                    }

                    if (ImGui::IsItemActive())
                        drawList->AddImage(m_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
                                           ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 96));
                    else if (ImGui::IsItemHovered())
                        drawList->AddImage(m_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
                                           ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
                    else
                        drawList->AddImage(m_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
                                           ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 160));
                } else {
                    ImGui::Dummy(ImVec2((float) restoreIconWidth, (float) restoreIconHeight));
                    drawList->AddImage(m_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0),
                                       ImVec2(1, 1), IM_COL32(255, 255, 255, 32));
                }

                ImGui::SameLine(0, 0);
# if IMGUI_VERSION_NUM < 18967
                ImGui::SetItemAllowOverlap();
# endif
                ImGui::Dummy(ImVec2(0, (float) restoreIconHeight));

                ImGui::PopID();
            }
        } // End if !blueprint.expired()
        ImGui::Unindent();

        static int changeCount = 0;

        ImGui::GetWindowDrawList()->AddRectFilled(
                ImGui::GetCursorScreenPos(),
                ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
                ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
        ImGui::Spacing(); ImGui::SameLine();
        ImGui::TextUnformatted("Selection");

        ImGui::BeginHorizontal("Selection Stats", ImVec2(paneWidth, 0));
        ImGui::Text("Changed %d time%s", changeCount, changeCount > 1 ? "s" : "");
        ImGui::Spring();
        if (ImGui::Button("Deselect All"))
            ed::ClearSelection();
        ImGui::EndHorizontal();
        ImGui::Indent();
        for (int i = 0; i < nodeCount; ++i) ImGui::Text("Node (%p)", selectedNodes[i].AsPointer());
        for (int i = 0; i < linkCount; ++i) ImGui::Text("Link (%p)", selectedLinks[i].AsPointer());
        ImGui::Unindent();

        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z))) {
            if(blueprint != nullptr) {
                auto links = blueprint->GetLinks();
                for (auto &link: links)
                    ed::Flow(link.ID);
            }
        }

        if (ed::HasSelectionChanged())
            ++changeCount;

        ImGui::EndChild();
    }

} // Slab::Blueprints