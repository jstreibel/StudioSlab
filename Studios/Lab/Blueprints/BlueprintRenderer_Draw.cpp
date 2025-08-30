//
// Created by joao on 11/2/24.
//

#include "BlueprintRenderer.h"

#include "3rdParty/ImGui.h"
#include "Core/Tools/Resources.h"
#include "Graphics/OpenGL/Images.h"

namespace Lab::Blueprints {
    static ImRect ImGui_GetItemRect()
    {
        return {ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
    }

    static ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
    {
        auto result = rect;
        result.Min.x -= x;
        result.Min.y -= y;
        result.Max.x += x;
        result.Max.y += y;
        return result;
    }

    bool FBlueprintRenderer::NotifyRender(const Graphics::FPlatformWindow&) {
        // m_Context->NewFrame();
        // // // ImGui::NewFrame();

        DoDrawing();

        // m_Context->Render();

        return true;
    }

    void FBlueprintRenderer::DoDrawing()
    {
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

                // Handle 'Blueprint' and 'Simple' node types.
                for (auto &node: m_Nodes) {
                    if (node.Type != NodeType::Blueprint && node.Type != NodeType::Simple)
                        continue;

                    HandleBlueprintOrSimpleNodeType(node);
                }

                // Handle 'Tree' node type
                for (auto &node: m_Nodes) {
                    if (node.Type != NodeType::Tree)
                        continue;

                    HandleTreeNodeType(node);
                }

                // Handle 'Houdini' node type
                for (auto &node: m_Nodes) {
                    if (node.Type != NodeType::Houdini)
                        continue;

                    HandleHoudiniNodeType(node);
                }

                // Handle 'Comment' node type
                for (auto &node: m_Nodes) {
                    if (node.Type != NodeType::Comment)
                        continue;

                    HandleCommentNodeType(node);
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
    }

    void FBlueprintRenderer::ShowOrdinals()
    {
        auto editorMin = ImGui::GetItemRectMin();
        auto editorMax = ImGui::GetItemRectMax();

        int nodeCount = Editor::GetNodeCount();
        std::vector<Editor::NodeId> orderedNodeIds;
        orderedNodeIds.resize(static_cast<size_t>(nodeCount));
        Editor::GetOrderedNodeIds(orderedNodeIds.data(), nodeCount);

        auto drawList = ImGui::GetWindowDrawList();
        drawList->PushClipRect(editorMin, editorMax);

        int ordinal = 0;
        for (auto &nodeId: orderedNodeIds) {
            auto p0 = Editor::GetNodePosition(nodeId);
            auto p1 = p0 + Editor::GetNodeSize(nodeId);
            p0 = Editor::CanvasToScreen(p0);
            p1 = Editor::CanvasToScreen(p1);


            ImGuiTextBuffer builder;
            builder.appendf("#%d", ordinal++);

            auto textSize = ImGui::CalcTextSize(builder.c_str());
            auto padding = ImVec2(2.0f, 2.0f);
            auto widgetSize = textSize + padding * 2;

            auto widgetPosition = ImVec2(p1.x, p0.y) + ImVec2(0.0f, -widgetSize.y);

            drawList->AddRectFilled(widgetPosition, widgetPosition + widgetSize, IM_COL32(100, 80, 80, 190),
                                    3.0f, ImDrawFlags_RoundCornersAll);
            drawList->AddRect(widgetPosition, widgetPosition + widgetSize, IM_COL32(200, 160, 160, 190),
                              3.0f, ImDrawFlags_RoundCornersAll);
            drawList->AddText(widgetPosition + padding, IM_COL32(255, 255, 255, 255), builder.c_str());
        }

        drawList->PopClipRect();
    }

    void FBlueprintRenderer::HandleBlueprintOrSimpleNodeType(FBlueprintNode& node)
    {
        if (node.Type != NodeType::Blueprint && node.Type != NodeType::Simple)
            return;

        const auto isSimple = node.Type == NodeType::Simple;

        bool hasOutputDelegates = false;
        for (auto &output: node.Outputs) if (output.Type == PinType::Delegate) hasOutputDelegates = true;

        const auto CanCreateLink = FBlueprint::CanCreateLink;
        const auto IsPinLinked = [this](const Editor::PinId id) { return Blueprint->IsPinLinked(id); };

        Builder.Begin(node.ID);
        if (!isSimple) {
            Builder.Header(node.Color);
            ImGui::Spring(0);
            ImGui::TextUnformatted(node.Name.c_str());
            ImGui::Spring(1);
            ImGui::Dummy(ImVec2(0, 28));
            if (hasOutputDelegates) {
                ImGui::BeginVertical("delegates", ImVec2(0, 28));
                ImGui::Spring(1, 0);
                for (auto &output: node.Outputs) {
                    if (output.Type != PinType::Delegate)
                        continue;

                    auto alpha = ImGui::GetStyle().Alpha;
                    if (MyState.NewLinkPin && !CanCreateLink(MyState.NewLinkPin, &output) && &output != MyState.NewLinkPin)
                        alpha = alpha * (48.0f / 255.0f);

                    Editor::BeginPin(output.ID, Editor::PinKind::Output);
                    Editor::PinPivotAlignment(ImVec2(1.0f, 0.5f));
                    Editor::PinPivotSize(ImVec2(0, 0));
                    ImGui::BeginHorizontal(output.ID.AsPointer());
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                    if (!output.Name.empty()) {
                        ImGui::TextUnformatted(output.Name.c_str());
                        ImGui::Spring(0);
                    }
                    DrawPinIcon(output, IsPinLinked(output.ID), (int) (alpha * 255));
                    ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
                    ImGui::EndHorizontal();
                    ImGui::PopStyleVar();
                    Editor::EndPin();

                    //DrawItemRect(ImColor(255, 0, 0));
                }
                ImGui::Spring(1, 0);
                ImGui::EndVertical();
                ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
            } else
                ImGui::Spring(0);
            Builder.EndHeader();
        }

        for (auto &input: node.Inputs) {
            auto alpha = ImGui::GetStyle().Alpha;
            if (MyState.NewLinkPin && !CanCreateLink(MyState.NewLinkPin, &input) && &input != MyState.NewLinkPin)
                alpha = alpha * (48.0f / 255.0f);

            Builder.Input(input.ID);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            DrawPinIcon(input, IsPinLinked(input.ID), (int) (alpha * 255));
            ImGui::Spring(0);
            if (!input.Name.empty()) {
                ImGui::TextUnformatted(input.Name.c_str());
                ImGui::Spring(0);
            }
            if (input.Type == PinType::Bool) {
                ImGui::Button("Hello");
                ImGui::Spring(0);
            }
            ImGui::PopStyleVar();
            Builder.EndInput();
        }

        if (isSimple) {
            Builder.Middle();

            ImGui::Spring(1, 0);
            ImGui::TextUnformatted(node.Name.c_str());
            ImGui::Spring(1, 0);
        }

        for (auto &output: node.Outputs) {
            if (!isSimple && output.Type == PinType::Delegate)
                continue;

            auto alpha = ImGui::GetStyle().Alpha;
            if (MyState.NewLinkPin && !CanCreateLink(MyState.NewLinkPin, &output) && &output != MyState.NewLinkPin)
                alpha = alpha * (48.0f / 255.0f);

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            Builder.Output(output.ID);
            if (output.Type == PinType::String) {
                static char buffer[128] = "Edit Me\nMultiline!";
                static bool wasActive = false;

                ImGui::PushItemWidth(100.0f);
                ImGui::InputText("##edit", buffer, 127);
                ImGui::PopItemWidth();
                if (ImGui::IsItemActive() && !wasActive) {
                    Editor::EnableShortcuts(false);
                    wasActive = true;
                } else if (!ImGui::IsItemActive() && wasActive) {
                    Editor::EnableShortcuts(true);
                    wasActive = false;
                }
                ImGui::Spring(0);
            }
            if (!output.Name.empty()) {
                ImGui::Spring(0);
                ImGui::TextUnformatted(output.Name.c_str());
            }
            ImGui::Spring(0);
            DrawPinIcon(output, IsPinLinked(output.ID), (int) (alpha * 255));
            ImGui::PopStyleVar();
            Builder.EndOutput();
        }

        Builder.End();
    }

    void FBlueprintRenderer::HandleTreeNodeType(FBlueprintNode& node) const
    {
        if (node.Type != NodeType::Tree)
            return;

        const float rounding = 5.0f;
        const float padding = 12.0f;

        const auto pinBackground = Editor::GetStyle().Colors[Editor::StyleColor_NodeBg];

        Editor::PushStyleColor(Editor::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
        Editor::PushStyleColor(Editor::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
        Editor::PushStyleColor(Editor::StyleColor_PinRect, ImColor(60, 180, 255, 150));
        Editor::PushStyleColor(Editor::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

        Editor::PushStyleVar(Editor::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
        Editor::PushStyleVar(Editor::StyleVar_NodeRounding, rounding);
        Editor::PushStyleVar(Editor::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
        Editor::PushStyleVar(Editor::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
        Editor::PushStyleVar(Editor::StyleVar_LinkStrength, 0.0f);
        Editor::PushStyleVar(Editor::StyleVar_PinBorderWidth, 1.0f);
        Editor::PushStyleVar(Editor::StyleVar_PinRadius, 5.0f);
        Editor::BeginNode(node.ID);

        ImGui::BeginVertical(node.ID.AsPointer());
        ImGui::BeginHorizontal("inputs");
        ImGui::Spring(0, padding * 2);

        ImRect inputsRect;
        int inputAlpha = 200;
        if (!node.Inputs.empty()) {
            auto &pin = node.Inputs[0];
            ImGui::Dummy(ImVec2(0, padding));
            ImGui::Spring(1, 0);
            inputsRect = ImGui_GetItemRect();

            Editor::PushStyleVar(Editor::StyleVar_PinArrowSize, 10.0f);
            Editor::PushStyleVar(Editor::StyleVar_PinArrowWidth, 10.0f);
#if IMGUI_VERSION_NUM > 18101
            Editor::PushStyleVar(Editor::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);
#else
            Editor::PushStyleVar(Editor::StyleVar_PinCorners, 12);
#endif
            Editor::BeginPin(pin.ID, Editor::PinKind::Input);
            Editor::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
            Editor::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
            Editor::EndPin();
            Editor::PopStyleVar(3);

            if (MyState.NewLinkPin && !FBlueprint::CanCreateLink(MyState.NewLinkPin, &pin) && &pin != MyState.NewLinkPin)
                inputAlpha = (int) (255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        } else
            ImGui::Dummy(ImVec2(0, padding));

        ImGui::Spring(0, padding * 2);
        ImGui::EndHorizontal();

        ImGui::BeginHorizontal("content_frame");
        ImGui::Spring(1, padding);

        ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
        ImGui::Dummy(ImVec2(160, 0));
        ImGui::Spring(1);
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::Spring(1);
        ImGui::EndVertical();
        auto contentRect = ImGui_GetItemRect();

        ImGui::Spring(1, padding);
        ImGui::EndHorizontal();

        ImGui::BeginHorizontal("outputs");
        ImGui::Spring(0, padding * 2);

        ImRect outputsRect;
        int outputAlpha = 200;
        if (!node.Outputs.empty()) {
            auto &pin = node.Outputs[0];
            ImGui::Dummy(ImVec2(0, padding));
            ImGui::Spring(1, 0);
            outputsRect = ImGui_GetItemRect();

#if IMGUI_VERSION_NUM > 18101
            Editor::PushStyleVar(Editor::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);
#else
            Editor::PushStyleVar(Editor::StyleVar_PinCorners, 3);
#endif
            Editor::BeginPin(pin.ID, Editor::PinKind::Output);
            Editor::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
            Editor::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
            Editor::EndPin();
            Editor::PopStyleVar();

            if (MyState.NewLinkPin && !FBlueprint::CanCreateLink(MyState.NewLinkPin, &pin) && &pin != MyState.NewLinkPin)
                outputAlpha = (int) (255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        } else
            ImGui::Dummy(ImVec2(0, padding));

        ImGui::Spring(0, padding * 2);
        ImGui::EndHorizontal();

        ImGui::EndVertical();

        Editor::EndNode();
        Editor::PopStyleVar(7);
        Editor::PopStyleColor(4);

        auto drawList = Editor::GetNodeBackgroundDrawList(node.ID);

        //const auto fringeScale = ImGui::GetStyle().AntiAliasFringeScale;
        //const auto unitSize    = 1.0f / fringeScale;

        //const auto ImDrawList_AddRect = [](ImDrawList* drawList, const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners, float thickness)
        //{
        //    if ((col >> 24) == 0)
        //        return;
        //    drawList->PathRect(a, b, rounding, rounding_corners);
        //    drawList->PathStroke(col, true, thickness);
        //};

#if IMGUI_VERSION_NUM > 18101
        const auto topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
        const auto bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;
#else
        const auto    topRoundCornersFlags = 1 | 2;
        const auto bottomRoundCornersFlags = 4 | 8;
#endif

        drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
                                IM_COL32((int) (255 * pinBackground.x), (int) (255 * pinBackground.y),
                                         (int) (255 * pinBackground.z), inputAlpha), 4.0f,
                                bottomRoundCornersFlags);
        //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
        drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
                          IM_COL32((int) (255 * pinBackground.x), (int) (255 * pinBackground.y),
                                   (int) (255 * pinBackground.z), inputAlpha), 4.0f,
                          bottomRoundCornersFlags);
        //ImGui::PopStyleVar();
        drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
                                IM_COL32((int) (255 * pinBackground.x), (int) (255 * pinBackground.y),
                                         (int) (255 * pinBackground.z), outputAlpha), 4.0f,
                                topRoundCornersFlags);
        //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
        drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
                          IM_COL32((int) (255 * pinBackground.x), (int) (255 * pinBackground.y),
                                   (int) (255 * pinBackground.z), outputAlpha), 4.0f,
                          topRoundCornersFlags);
        //ImGui::PopStyleVar();
        drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200),
                                0.0f);
        //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
        drawList->AddRect(
                contentRect.GetTL(),
                contentRect.GetBR(),
                IM_COL32(48, 128, 255, 100), 0.0f);
        //ImGui::PopStyleVar();
    }

    void FBlueprintRenderer::HandleHoudiniNodeType(FBlueprintNode& node) const
    {
        if (node.Type != NodeType::Houdini)
            return;

        const float rounding = 10.0f;
        const float padding = 12.0f;

        Editor::PushStyleColor(Editor::StyleColor_NodeBg, ImColor(229, 229, 229, 200));
        Editor::PushStyleColor(Editor::StyleColor_NodeBorder, ImColor(125, 125, 125, 200));
        Editor::PushStyleColor(Editor::StyleColor_PinRect, ImColor(229, 229, 229, 60));
        Editor::PushStyleColor(Editor::StyleColor_PinRectBorder, ImColor(125, 125, 125, 60));

        const auto pinBackground = Editor::GetStyle().Colors[Editor::StyleColor_NodeBg];

        Editor::PushStyleVar(Editor::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
        Editor::PushStyleVar(Editor::StyleVar_NodeRounding, rounding);
        Editor::PushStyleVar(Editor::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
        Editor::PushStyleVar(Editor::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
        Editor::PushStyleVar(Editor::StyleVar_LinkStrength, 0.0f);
        Editor::PushStyleVar(Editor::StyleVar_PinBorderWidth, 1.0f);
        Editor::PushStyleVar(Editor::StyleVar_PinRadius, 6.0f);
        Editor::BeginNode(node.ID);

        ImGui::BeginVertical(node.ID.AsPointer());
        if (!node.Inputs.empty()) {
            ImGui::BeginHorizontal("inputs");
            ImGui::Spring(1, 0);

            ImRect inputsRect;
            int inputAlpha = 200;
            for (auto &pin: node.Inputs) {
                ImGui::Dummy(ImVec2(padding, padding));
                inputsRect = ImGui_GetItemRect();
                ImGui::Spring(1, 0);
                inputsRect.Min.y -= padding;
                inputsRect.Max.y -= padding;

#if IMGUI_VERSION_NUM > 18101
                const auto allRoundCornersFlags = ImDrawFlags_RoundCornersAll;
#else
                const auto allRoundCornersFlags = 15;
#endif
                //Editor::PushStyleVar(Editor::StyleVar_PinArrowSize, 10.0f);
                //Editor::PushStyleVar(Editor::StyleVar_PinArrowWidth, 10.0f);
                Editor::PushStyleVar(Editor::StyleVar_PinCorners, allRoundCornersFlags);

                Editor::BeginPin(pin.ID, Editor::PinKind::Input);
                Editor::PinPivotRect(inputsRect.GetCenter(), inputsRect.GetCenter());
                Editor::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
                Editor::EndPin();
                //Editor::PopStyleVar(3);
                Editor::PopStyleVar(1);

                auto drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(inputsRect.GetTL(), inputsRect.GetBR(),
                                        IM_COL32((int) (255 * pinBackground.x),
                                                 (int) (255 * pinBackground.y),
                                                 (int) (255 * pinBackground.z), inputAlpha), 4.0f,
                                        allRoundCornersFlags);
                drawList->AddRect(inputsRect.GetTL(), inputsRect.GetBR(),
                                  IM_COL32((int) (255 * pinBackground.x), (int) (255 * pinBackground.y),
                                           (int) (255 * pinBackground.z), inputAlpha), 4.0f,
                                  allRoundCornersFlags);

                if (MyState.NewLinkPin && !FBlueprint::CanCreateLink(MyState.NewLinkPin, &pin) && &pin != MyState.NewLinkPin)
                    inputAlpha = (int) (255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
            }

            //ImGui::Spring(1, 0);
            ImGui::EndHorizontal();
        }

        ImGui::BeginHorizontal("content_frame");
        ImGui::Spring(1, padding);

        ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
        ImGui::Dummy(ImVec2(160, 0));
        ImGui::Spring(1);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::PopStyleColor();
        ImGui::Spring(1);
        ImGui::EndVertical();

        ImGui::Spring(1, padding);
        ImGui::EndHorizontal();

        if (!node.Outputs.empty()) {
            ImGui::BeginHorizontal("outputs");
            ImGui::Spring(1, 0);

            ImRect outputsRect;
            int outputAlpha = 200;
            for (auto &pin: node.Outputs) {
                ImGui::Dummy(ImVec2(padding, padding));
                outputsRect = ImGui_GetItemRect();
                ImGui::Spring(1, 0);
                outputsRect.Min.y += padding;
                outputsRect.Max.y += padding;

#if IMGUI_VERSION_NUM > 18101
                constexpr auto AllRoundCornersFlags = ImDrawFlags_RoundCornersAll;
                constexpr auto TopRoundCornersFlags = ImDrawFlags_RoundCornersTop;
#else
                const auto allRoundCornersFlags = 15;
                const auto topRoundCornersFlags = 3;
#endif

                Editor::PushStyleVar(Editor::StyleVar_PinCorners, TopRoundCornersFlags);
                Editor::BeginPin(pin.ID, Editor::PinKind::Output);
                Editor::PinPivotRect(outputsRect.GetCenter(), outputsRect.GetCenter());
                Editor::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
                Editor::EndPin();
                Editor::PopStyleVar();


                auto drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR(),
                                        IM_COL32((int) (255 * pinBackground.x),
                                                 (int) (255 * pinBackground.y),
                                                 (int) (255 * pinBackground.z), outputAlpha), 4.0f,
                                        AllRoundCornersFlags);
                drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR(),
                                  IM_COL32((int) (255 * pinBackground.x), (int) (255 * pinBackground.y),
                                           (int) (255 * pinBackground.z), outputAlpha), 4.0f,
                                  AllRoundCornersFlags);


                if (MyState.NewLinkPin && !FBlueprint::CanCreateLink(MyState.NewLinkPin, &pin) && &pin != MyState.NewLinkPin)
                    outputAlpha = (int) (255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
            }

            ImGui::EndHorizontal();
        }

        ImGui::EndVertical();

        Editor::EndNode();
        Editor::PopStyleVar(7);
        Editor::PopStyleColor(4);
    }

    void FBlueprintRenderer::HandleCommentNodeType(FBlueprintNode& node)
    {
        if (node.Type != NodeType::Comment)
            return;

        const float commentAlpha = 0.75f;

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
        Editor::PushStyleColor(Editor::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
        Editor::PushStyleColor(Editor::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
        Editor::BeginNode(node.ID);
        ImGui::PushID(node.ID.AsPointer());
        ImGui::BeginVertical("content");
        ImGui::BeginHorizontal("horizontal");
        ImGui::Spring(1);
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::Spring(1);
        ImGui::EndHorizontal();
        Editor::Group(node.Size);
        ImGui::EndVertical();
        ImGui::PopID();
        Editor::EndNode();
        Editor::PopStyleColor(2);
        ImGui::PopStyleVar();

        if (Editor::BeginGroupHint(node.ID)) {
            //auto alpha   = static_cast<int>(commentAlpha * ImGui::GetStyle().Alpha * 255);
            auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

            //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

            auto min = Editor::GetGroupMin();
            //auto max = Editor::GetGroupMax();

            ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
            ImGui::BeginGroup();
            ImGui::TextUnformatted(node.Name.c_str());
            ImGui::EndGroup();

            auto drawList = Editor::GetHintBackgroundDrawList();

            auto hintBounds = ImGui_GetItemRect();
            auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

            drawList->AddRectFilled(
                    hintFrameBounds.GetTL(),
                    hintFrameBounds.GetBR(),
                    IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

            drawList->AddRect(
                    hintFrameBounds.GetTL(),
                    hintFrameBounds.GetBR(),
                    IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

            //ImGui::PopStyleVar();
        }
        Editor::EndGroupHint();
    }

    void FBlueprintRenderer::ShowContextMenus()
    {
        auto openPopupPosition = ImGui::GetMousePos();
        Editor::Suspend();
        if      (Editor::ShowNodeContextMenu(&MyState.ContextNodeId)) ImGui::OpenPopup("Node Context Menu");
        else if (Editor::ShowPinContextMenu (&MyState.ContextPinId))  ImGui::OpenPopup("Pin Context Menu");
        else if (Editor::ShowLinkContextMenu(&MyState.ContextLinkId)) ImGui::OpenPopup("Link Context Menu");
        else if (Editor::ShowBackgroundContextMenu())       { ImGui::OpenPopup("Create New Node"); MyState.NewNodeLinkPin = nullptr; }
        Editor::Resume();

        Editor::Suspend();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

        if (ImGui::BeginPopup("Node Context Menu")) {
            auto node = Blueprint->FindNode(MyState.ContextNodeId);

            ImGui::TextUnformatted("Node Context Menu");
            ImGui::Separator();
            if (node) {
                ImGui::Text("ID: %p", node->ID.AsPointer());
                ImGui::Text("Type: %s",
                            node->Type == NodeType::Blueprint ? "Blueprint" : (node->Type == NodeType::Tree
                                                                               ? "Tree" : "Comment"));
                ImGui::Text("Inputs: %d", (int) node->Inputs.size());
                ImGui::Text("Outputs: %d", (int) node->Outputs.size());
            } else
                ImGui::Text("Unknown node: %p", MyState.ContextNodeId.AsPointer());
            ImGui::Separator();
            if (ImGui::MenuItem("Delete"))
                Editor::DeleteNode(MyState.ContextNodeId);
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Pin Context Menu")) {
            auto pin = Blueprint->FindPin(MyState.ContextPinId);

            ImGui::TextUnformatted("Pin Context Menu");
            ImGui::Separator();
            if (pin) {
                ImGui::Text("ID: %p", pin->ID.AsPointer());
                if (pin->Node)
                    ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
                else
                    ImGui::Text("Node: %s", "<none>");
            } else
                ImGui::Text("Unknown pin: %p", MyState.ContextPinId.AsPointer());

            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Link Context Menu")) {
            auto link = Blueprint->FindLink(MyState.ContextLinkId);

            ImGui::TextUnformatted("Link Context Menu");
            ImGui::Separator();
            if (link) {
                ImGui::Text("ID: %p", link->ID.AsPointer());
                ImGui::Text("From: %p", link->StartPinID.AsPointer());
                ImGui::Text("To: %p", link->EndPinID.AsPointer());
            } else
                ImGui::Text("Unknown link: %p", MyState.ContextLinkId.AsPointer());
            ImGui::Separator();
            if (ImGui::MenuItem("Delete"))
                Editor::DeleteLink(MyState.ContextLinkId);
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Create New Node")) {
            auto newNodePosition = openPopupPosition;

            FBlueprintNode *node = nullptr;
            for (auto &Spawner : Blueprint->GetNodeSpawners())
            {
                fix NodeClass = Spawner.first;
                if (ImGui::MenuItem(NodeClass.c_str())) node = Blueprint->SpawnNode(NodeClass);
            }

            if (node) {
                Blueprint->BuildNodes();

                MyState.bCreateNewNode = false;

                Editor::SetNodePosition(node->ID, newNodePosition);

                if (auto startPin = MyState.NewNodeLinkPin) {
                    auto &pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

                    auto m_Links = Blueprint->GetLinks();

                    for (auto &pin: pins) {
                        if (FBlueprint::CanCreateLink(startPin, &pin)) {
                            auto endPin = &pin;
                            if (startPin->Kind == PinKind::Input)
                                std::swap(startPin, endPin);

                            m_Links.emplace_back(FBlueprint::GetNextId(), startPin->ID, endPin->ID);
                            m_Links.back().Color = GetIconColor(startPin->Type);

                            break;
                        }
                    }
                }
            }

            ImGui::EndPopup();
        }
        else
        {
            MyState.bCreateNewNode = false;
        }
        ImGui::PopStyleVar();
        Editor::Resume();
    }
}
