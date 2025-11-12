//
// Created by joao on 11/2/24.
//

#include "BlueprintRenderer.h"

#include <ranges>
#include <utility>

#include "Utilities/drawing.h"
#include "Utilities/widgets.h"
#include "Core/Tools/Resources.h"
#include "StudioSlab.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/OpenGL/RawTextures.h"


namespace Lab::Blueprints {

    using namespace ax;

    using ax::Widgets::IconType;

    FBlueprintRenderer::FBlueprintRenderer(TPointer<FBlueprint> blueprint, TPointer<Graphics::FImGuiContext> GUIContext )
    : Blueprint(std::move(blueprint))
    {
        assert(GUIContext != nullptr);

        m_Context = DynamicPointerCast<Graphics::FImGuiContext>(std::move(GUIContext));

        Editor::Config config;

        config.SettingsFile = "Blueprints.json";

        config.UserPointer = this;

        config.LoadNodeSettings = [](Editor::NodeId nodeId, char* data, void* userPointer) -> size_t
        {
            auto self = static_cast<FBlueprintRenderer*>(userPointer);

            auto node = self->Blueprint->FindNode(nodeId);
            if (!node)
                return 0;

            if (data != nullptr)
                memcpy((void*)data, (void*)(node->State.data()), node->State.size());
            return node->State.size();
        };

        config.SaveNodeSettings = [](Editor::NodeId nodeId, const char* data,
                size_t size, Editor::SaveReasonFlags reason, void* userPointer) -> bool
        {
            auto self = static_cast<FBlueprintRenderer*>(userPointer);

            auto node = self->Blueprint->FindNode(nodeId);
            if (!node)
                return false;

            node->State.assign(data, size);

            self->TouchNode(nodeId);

            return true;
        };

        m_Editor = Editor::CreateEditor(&config);
        Editor::SetCurrentEditor(m_Editor);

        auto location = Core::Resources::Folder + "Blueprints/";
        m_HeaderBackground = Graphics::SlabTexture::LoadTextureFromImage(location + "BlueprintBackground.png");
        m_SaveIcon         = Graphics::SlabTexture::LoadTextureFromImage(location + "ic_save_white_24dp.png");
        m_RestoreIcon      = Graphics::SlabTexture::LoadTextureFromImage(location + "ic_restore_white_24dp.png");

        Builder = EditorUtil::BlueprintNodeBuilder(m_HeaderBackground, Graphics::SlabTexture::GetTextureWidth(m_HeaderBackground), Graphics::SlabTexture::GetTextureHeight(m_HeaderBackground));
    }

    void FBlueprintRenderer::TouchNode(const Editor::NodeId id) { m_NodeTouchTime[id] = m_TouchTime; }

    float FBlueprintRenderer::GetTouchProgress(const Editor::NodeId id) {
        auto it = m_NodeTouchTime.find(id);
        if (it != m_NodeTouchTime.end() && it->second > 0.0f)
            return (m_TouchTime - it->second) / m_TouchTime;

        return 0.0f;
    }

    void FBlueprintRenderer::UpdateTouch() {
        const auto DeltaTime = ImGui::GetIO().DeltaTime;
        for (auto& Val : m_NodeTouchTime | std::views::values) if (Val > 0.0f) Val -= DeltaTime;
    }

    bool FBlueprintRenderer::NotifyRender(const Graphics::FPlatformWindow&) {

        constexpr auto WindowFlags = 0;

        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_Once);
        ImGui::SetNextWindowSize({2400, 1350}, ImGuiCond_Once);
        const auto windowBorderSize = ImGui::GetStyle().WindowBorderSize;
        const auto windowRounding   = ImGui::GetStyle().WindowRounding;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        ImGui::Begin("Content", nullptr, WindowFlags);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, windowBorderSize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, windowRounding);

        // "OnFrame(io.DeltaTime);":
        {
            UpdateTouch();

            // ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

            Editor::SetCurrentEditor(m_Editor);

            Splitter(true, 4.0f, &MyState.LeftPaneWidth, &MyState.RightPaneWidth, 50.0f, 50.0f);

            ShowLeftPane(MyState.LeftPaneWidth - 4.0f);

            ImGui::SameLine(0.0f, 12.0f);

            /**************************************************/
            /**************************************************/
            /****** ALGUMAS DEFINICOES ************************/
            if(Blueprint == nullptr) NOT_IMPLEMENTED

            auto &m_Nodes = Blueprint->GetNodes();
            auto &m_Links = Blueprint->GetLinks();
            /**************************************************/
            /**************************************************/
            /**************************************************/

            Editor::Begin("Node editor");
            {
                const auto CursorTopLeft = ImGui::GetCursorScreenPos();

                for (auto &node: m_Nodes) {
                    switch (node.Type)
                    {
                    case NodeType::Blueprint:
                    case NodeType::Simple:
                        HandleBlueprintOrSimpleNodeType(node);
                        break;
                    case NodeType::Tree:
                        HandleTreeNodeType(node);
                        break;
                    case NodeType::Houdini:
                        HandleHoudiniNodeType(node);
                        break;
                    case NodeType::Comment:
                        HandleCommentNodeType(node);
                        break;
                    }
                }

                // Handle links
                for (auto &link: m_Links) Editor::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);

                // Handle node creation
                HandleNodeCreation();

                ImGui::SetCursorScreenPos(CursorTopLeft);
            }

            ShowContextMenus();

            Editor::End();

            if (m_ShowOrdinals) ShowOrdinals();
        }

        ImGui::PopStyleVar(2);
        ImGui::End();
        ImGui::PopStyleVar(2);

        return true;
    }

    void FBlueprintRenderer::HandleNodeCreation()
    {
        auto FindPin = [this](NodeEditor::PinId id)  { return Blueprint->FindPin(id); };
        auto &m_Links = Blueprint->GetLinks();
        auto &m_Nodes = Blueprint->GetNodes();

        if (!MyState.bCreateNewNode)
        {
            if (Editor::BeginCreate(ImColor(255, 255, 255), 2.0f))
            {
                auto ShowLabel = [](const char *label, ImColor color)
                {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                    const auto size = ImGui::CalcTextSize(label);

                    const auto padding = ImGui::GetStyle().FramePadding;
                    const auto spacing = ImGui::GetStyle().ItemSpacing;

                    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                    const auto rectMin = ImGui::GetCursorScreenPos() - padding;
                    const auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                    const auto drawList = ImGui::GetWindowDrawList();
                    drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                    ImGui::TextUnformatted(label);
                };

                Editor::PinId startPinId = 0, endPinId = 0;
                if (Editor::QueryNewLink(&startPinId, &endPinId)) {
                    auto StartPin = FindPin(startPinId);
                    auto EndPin = FindPin(endPinId);

                    MyState.NewLinkPin = StartPin ? StartPin : EndPin;

                    assert(StartPin != nullptr && EndPin != nullptr);
                    if (StartPin->Kind == PinKind::Input) {
                        std::swap(StartPin, EndPin);
                        std::swap(startPinId, endPinId);
                    }

                    if (StartPin && EndPin) {
                        if (EndPin == StartPin) {
                            Editor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (EndPin->Kind == StartPin->Kind) {
                            ShowLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                            Editor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                            //else if (endPin->Node == startPin->Node)
                            //{
                            //    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                            //    Editor::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                            //}
                        else if (EndPin->Type != StartPin->Type) {
                            ShowLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                            Editor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                        }
                        else {
                            ShowLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (Editor::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                                m_Links.emplace_back(Link(FBlueprint::GetNextId(), startPinId, endPinId));
                                m_Links.back().Color = GetIconColor(StartPin->Type);
                            }
                        }
                    }
                }

                Editor::PinId pinId = 0;
                if (Editor::QueryNewNode(&pinId)) {
                    MyState.NewLinkPin = FindPin(pinId);
                    if (MyState.NewLinkPin)
                        ShowLabel("+ Create Node", ImColor(32, 45, 32, 180));

                    if (Editor::AcceptNewItem()) {
                        MyState.bCreateNewNode = true;
                        MyState.NewNodeLinkPin = FindPin(pinId);
                        MyState.NewLinkPin = nullptr;
                        Editor::Suspend();
                        ImGui::OpenPopup("Create New Node");
                        Editor::Resume();
                    }
                }
            }
            else
            {
                MyState.NewLinkPin = nullptr;
            }

            Editor::EndCreate();

            if (Editor::BeginDelete()) {
                Editor::NodeId nodeId = 0;
                while (Editor::QueryDeletedNode(&nodeId)) {
                    if (Editor::AcceptDeletedItem()) {
                        auto id = std::find_if(m_Nodes.begin(), m_Nodes.end(),
                                               [nodeId](auto &node) { return node.ID == nodeId; });
                        if (id != m_Nodes.end())
                            m_Nodes.erase(id);
                    }
                }

                Editor::LinkId linkId = 0;
                while (Editor::QueryDeletedLink(&linkId)) {
                    if (Editor::AcceptDeletedItem()) {
                        auto id = std::find_if(m_Links.begin(), m_Links.end(),
                                               [linkId](auto &link) { return link.ID == linkId; });
                        if (id != m_Links.end())
                            m_Links.erase(id);
                    }
                }
            }
            Editor::EndDelete();
        }
    }

    ImColor FBlueprintRenderer::GetIconColor(PinType type) {
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

    void FBlueprintRenderer::DrawPinIcon(const Pin &pin, const bool connected, const int alpha) const
    {
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

    void FBlueprintRenderer::ShowStyleEditor(bool *show) {
        if (!ImGui::Begin("Style", show))
        {
            ImGui::End();
            return;
        }

        auto paneWidth = ImGui::GetContentRegionAvail().x;

        auto& editorStyle = Editor::GetStyle();
        ImGui::BeginHorizontal("Style buttons", ImVec2(paneWidth, 0), 1.0f);
        ImGui::TextUnformatted("Values");
        ImGui::Spring();
        if (ImGui::Button("Reset to defaults"))
            editorStyle = Editor::Style();
        ImGui::EndHorizontal();
        ImGui::Spacing();
        ImGui::DragFloat4("Node Padding",               &editorStyle.NodePadding.x, 0.1f, 0.0f, 40.0f);
        ImGui::DragFloat("Node Rounding",               &editorStyle.NodeRounding, 0.1f, 0.0f, 40.0f);
        ImGui::DragFloat("Node Border Width",           &editorStyle.NodeBorderWidth, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat("Hovered Node Border Width",   &editorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat("Hovered Node Border Offset",  &editorStyle.HoverNodeBorderOffset, 0.1f, -40.0f, 40.0f);
        ImGui::DragFloat("Selected Node Border Width",  &editorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat("Selected Node Border Offset", &editorStyle.SelectedNodeBorderOffset, 0.1f, -40.0f, 40.0f);
        ImGui::DragFloat("Pin Rounding",                &editorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
        ImGui::DragFloat("Pin Border Width",            &editorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat("Link Strength",               &editorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
        //ImVec2  SourceDirection;
        //ImVec2  TargetDirection;
        ImGui::DragFloat("Scroll Duration",             &editorStyle.ScrollDuration, 0.001f, 0.0f, 2.0f);
        ImGui::DragFloat("Flow Marker Distance",        &editorStyle.FlowMarkerDistance, 1.0f, 1.0f, 200.0f);
        ImGui::DragFloat("Flow Speed",                  &editorStyle.FlowSpeed, 1.0f, 1.0f, 2000.0f);
        ImGui::DragFloat("Flow Duration",               &editorStyle.FlowDuration, 0.001f, 0.0f, 5.0f);
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
        for (int i = 0; i < Editor::StyleColor_Count; ++i)
        {
            auto name = Editor::GetStyleColorName((Editor::StyleColor)i);
            if (!filter.PassFilter(name))
                continue;

            ImGui::ColorEdit4(name, &editorStyle.Colors[i].x, edit_mode);
        }
        ImGui::PopItemWidth();

        ImGui::End();
    }

    void FBlueprintRenderer::ShowLeftPane(float paneWidth) {
        auto& io = ImGui::GetIO();

        ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

        paneWidth = ImGui::GetContentRegionAvail().x;

        static bool showStyleEditor = false;
        ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
        ImGui::Spring(0.0f, 0.0f);
        if (ImGui::Button("Zoom to Content"))
            Editor::NavigateToContent();
        ImGui::Spring(0.0f);
        if (ImGui::Button("Show Flow"))
        {
            if(Blueprint != nullptr) {
                auto links = Blueprint->GetLinks();

                for (auto &link: links)
                    Editor::Flow(link.ID);
            }
        }
        ImGui::Spring();
        if constexpr (false) if (ImGui::Button("Edit Style")) showStyleEditor = true;
        ImGui::EndHorizontal();
        if constexpr (false) ImGui::Checkbox("Show Ordinals", &m_ShowOrdinals);

        if (showStyleEditor) ShowStyleEditor(&showStyleEditor);

        std::vector<Editor::NodeId> selectedNodes;
        std::vector<Editor::LinkId> selectedLinks;
        selectedNodes.resize(Editor::GetSelectedObjectCount());
        selectedLinks.resize(Editor::GetSelectedObjectCount());

        int nodeCount = Editor::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
        int linkCount = Editor::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));

        selectedNodes.resize(nodeCount);
        selectedLinks.resize(linkCount);

        int saveIconWidth     = Graphics::SlabTexture::GetTextureWidth(m_SaveIcon);
        int saveIconHeight    = Graphics::SlabTexture::GetTextureWidth(m_SaveIcon);
        int restoreIconWidth  = Graphics::SlabTexture::GetTextureWidth(m_RestoreIcon);
        int restoreIconHeight = Graphics::SlabTexture::GetTextureWidth(m_RestoreIcon);

        ImGui::GetWindowDrawList()->AddRectFilled(
                ImGui::GetCursorScreenPos(),
                ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
                ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
        ImGui::Spacing(); ImGui::SameLine();
        ImGui::TextUnformatted("Nodes");
        ImGui::Indent();
        if(Blueprint != nullptr) {
            auto nodes = Blueprint->GetNodes();

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
                            Editor::SelectNode(node.ID, true);
                        else
                            Editor::DeselectNode(node.ID);
                    } else
                        Editor::SelectNode(node.ID, false);

                    Editor::NavigateToSelection();
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
                        Editor::RestoreNodeState(node.ID);
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
            Editor::ClearSelection();
        ImGui::EndHorizontal();
        ImGui::Indent();
        for (int i = 0; i < nodeCount; ++i) ImGui::Text("Node (%p)", selectedNodes[i].AsPointer());
        for (int i = 0; i < linkCount; ++i) ImGui::Text("Link (%p)", selectedLinks[i].AsPointer());
        ImGui::Unindent();

        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z))) {
            if(Blueprint != nullptr) {
                auto links = Blueprint->GetLinks();
                for (auto &link: links)
                    Editor::Flow(link.ID);
            }
        }

        if (Editor::HasSelectionChanged())
            ++changeCount;

        ImGui::EndChild();
    }

} // Slab::Blueprints