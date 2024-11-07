//
// Created by joao on 11/2/24.
//

#include "BlueprintRenderer.h"

#include "3rdParty/ImGui.h"

namespace Slab::Blueprints {
    static inline ImRect ImGui_GetItemRect()
    {
        return {ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
    }

    static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
    {
        auto result = rect;
        result.Min.x -= x;
        result.Min.y -= y;
        result.Max.x += x;
        result.Max.y += y;
        return result;
    }

    void BlueprintRenderer::Draw() {
        m_Context->NewFrame();
        // // ImGui::NewFrame();

        DoDrawing();

        m_Context->notifyRender();
    }

    void BlueprintRenderer::DoDrawing() {
        // auto& io = ImGui::GetIO();
        auto window_flags = 0;// ImGuiWindowFlags_NoTitleBar |
        // ImGuiWindowFlags_NoResize |
        // ImGuiWindowFlags_NoMove |
        // ImGuiWindowFlags_NoScrollbar |
        // ImGuiWindowFlags_NoScrollWithMouse |
        // ImGuiWindowFlags_NoSavedSettings |
        // ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_Once);
        // ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::SetNextWindowSize({2400, 1350}, ImGuiCond_Once);
        const auto windowBorderSize = ImGui::GetStyle().WindowBorderSize;
        const auto windowRounding   = ImGui::GetStyle().WindowRounding;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::Begin("Content", nullptr, window_flags);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, windowBorderSize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, windowRounding);

        // "OnFrame(io.DeltaTime);":
        {

            UpdateTouch();

            auto &io = ImGui::GetIO();

            ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

            ed::SetCurrentEditor(m_Editor);

            //auto& style = ImGui::GetStyle();

# if 0
            {
                for (auto x = -io.DisplaySize.y; x < io.DisplaySize.x; x += 10.0f)
                {
                    ImGui::GetWindowDrawList()->AddLine(ImVec2(x, 0), ImVec2(x + io.DisplaySize.y, io.DisplaySize.y),
                        IM_COL32(255, 255, 0, 255));
                }
            }
# endif

            static ed::NodeId contextNodeId = 0;
            static ed::LinkId contextLinkId = 0;
            static ed::PinId contextPinId = 0;
            static bool createNewNode = false;
            static Pin *newNodeLinkPin = nullptr;
            static Pin *newLinkPin = nullptr;

            static float leftPaneWidth = 400.0f;
            static float rightPaneWidth = 800.0f;
            Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

            ShowLeftPane(leftPaneWidth - 4.0f);

            ImGui::SameLine(0.0f, 12.0f);

            /**************************************************/
            /**************************************************/
            /****** ALGUMAS DEFINICOES ************************/
            if(blueprint == nullptr) NOT_IMPLEMENTED

            auto &m_Nodes = blueprint->GetNodes();
            auto &m_Links = blueprint->GetLinks();
            auto CanCreateLink = [this](Pin*a, Pin*b){
                return blueprint->CanCreateLink(a,b);
            };
            auto IsPinLinked   = [this](ax::NodeEditor::PinId  id) {
                return blueprint->IsPinLinked(id);
            };
            auto FindPin       = [this](ax::NodeEditor::PinId id) {
                return blueprint->FindPin(id);
            };
            auto FindNode      = [this](ax::NodeEditor::NodeId id) {
                return blueprint->FindNode(id);
            };
            auto FindLink      = [this](ax::NodeEditor::LinkId id) {
                return blueprint->FindLink(id);
            };
            auto GetNextId     = [this]() {
                return blueprint->GetNextId();
            };
            /**************************************************/
            /**************************************************/
            /**************************************************/

            ed::Begin("Node editor");
            {
                auto cursorTopLeft = ImGui::GetCursorScreenPos();

                util::BlueprintNodeBuilder builder(m_HeaderBackground,
                                                   Slab::Graphics::GetTextureWidth(m_HeaderBackground),
                                                   Slab::Graphics::GetTextureHeight(m_HeaderBackground));

                for (auto &node: m_Nodes) {
                    if (node.Type != NodeType::Blueprint && node.Type != NodeType::Simple)
                        continue;

                    const auto isSimple = node.Type == NodeType::Simple;

                    bool hasOutputDelegates = false;
                    for (auto &output: node.Outputs)
                        if (output.Type == PinType::Delegate)
                            hasOutputDelegates = true;

                    builder.Begin(node.ID);
                    if (!isSimple) {
                        builder.Header(node.Color);
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
                                if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
                                    alpha = alpha * (48.0f / 255.0f);

                                ed::BeginPin(output.ID, ed::PinKind::Output);
                                ed::PinPivotAlignment(ImVec2(1.0f, 0.5f));
                                ed::PinPivotSize(ImVec2(0, 0));
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
                                ed::EndPin();

                                //DrawItemRect(ImColor(255, 0, 0));
                            }
                            ImGui::Spring(1, 0);
                            ImGui::EndVertical();
                            ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
                        } else
                            ImGui::Spring(0);
                        builder.EndHeader();
                    }

                    for (auto &input: node.Inputs) {
                        auto alpha = ImGui::GetStyle().Alpha;
                        if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
                            alpha = alpha * (48.0f / 255.0f);

                        builder.Input(input.ID);
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
                        builder.EndInput();
                    }

                    if (isSimple) {
                        builder.Middle();

                        ImGui::Spring(1, 0);
                        ImGui::TextUnformatted(node.Name.c_str());
                        ImGui::Spring(1, 0);
                    }

                    for (auto &output: node.Outputs) {
                        if (!isSimple && output.Type == PinType::Delegate)
                            continue;

                        auto alpha = ImGui::GetStyle().Alpha;
                        if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
                            alpha = alpha * (48.0f / 255.0f);

                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                        builder.Output(output.ID);
                        if (output.Type == PinType::String) {
                            static char buffer[128] = "Edit Me\nMultiline!";
                            static bool wasActive = false;

                            ImGui::PushItemWidth(100.0f);
                            ImGui::InputText("##edit", buffer, 127);
                            ImGui::PopItemWidth();
                            if (ImGui::IsItemActive() && !wasActive) {
                                ed::EnableShortcuts(false);
                                wasActive = true;
                            } else if (!ImGui::IsItemActive() && wasActive) {
                                ed::EnableShortcuts(true);
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
                        builder.EndOutput();
                    }

                    builder.End();
                }

                for (auto &node: m_Nodes) {
                    if (node.Type != NodeType::Tree)
                        continue;

                    const float rounding = 5.0f;
                    const float padding = 12.0f;

                    const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

                    ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
                    ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
                    ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));
                    ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

                    ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
                    ed::PushStyleVar(ed::StyleVar_NodeRounding, rounding);
                    ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
                    ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
                    ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
                    ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
                    ed::PushStyleVar(ed::StyleVar_PinRadius, 5.0f);
                    ed::BeginNode(node.ID);

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

                        ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
                        ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
#if IMGUI_VERSION_NUM > 18101
                        ed::PushStyleVar(ed::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);
#else
                        ed::PushStyleVar(ed::StyleVar_PinCorners, 12);
#endif
                        ed::BeginPin(pin.ID, ed::PinKind::Input);
                        ed::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
                        ed::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
                        ed::EndPin();
                        ed::PopStyleVar(3);

                        if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
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
                        ed::PushStyleVar(ed::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);
#else
                        ed::PushStyleVar(ed::StyleVar_PinCorners, 3);
#endif
                        ed::BeginPin(pin.ID, ed::PinKind::Output);
                        ed::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
                        ed::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
                        ed::EndPin();
                        ed::PopStyleVar();

                        if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
                            outputAlpha = (int) (255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
                    } else
                        ImGui::Dummy(ImVec2(0, padding));

                    ImGui::Spring(0, padding * 2);
                    ImGui::EndHorizontal();

                    ImGui::EndVertical();

                    ed::EndNode();
                    ed::PopStyleVar(7);
                    ed::PopStyleColor(4);

                    auto drawList = ed::GetNodeBackgroundDrawList(node.ID);

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

                for (auto &node: m_Nodes) {
                    if (node.Type != NodeType::Houdini)
                        continue;

                    const float rounding = 10.0f;
                    const float padding = 12.0f;


                    ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(229, 229, 229, 200));
                    ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(125, 125, 125, 200));
                    ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(229, 229, 229, 60));
                    ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(125, 125, 125, 60));

                    const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

                    ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
                    ed::PushStyleVar(ed::StyleVar_NodeRounding, rounding);
                    ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
                    ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
                    ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
                    ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
                    ed::PushStyleVar(ed::StyleVar_PinRadius, 6.0f);
                    ed::BeginNode(node.ID);

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
                            //ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
                            //ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
                            ed::PushStyleVar(ed::StyleVar_PinCorners, allRoundCornersFlags);

                            ed::BeginPin(pin.ID, ed::PinKind::Input);
                            ed::PinPivotRect(inputsRect.GetCenter(), inputsRect.GetCenter());
                            ed::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
                            ed::EndPin();
                            //ed::PopStyleVar(3);
                            ed::PopStyleVar(1);

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

                            if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
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
                    auto contentRect = ImGui_GetItemRect();

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
                            const auto allRoundCornersFlags = ImDrawFlags_RoundCornersAll;
                            const auto topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
#else
                            const auto allRoundCornersFlags = 15;
                            const auto topRoundCornersFlags = 3;
#endif

                            ed::PushStyleVar(ed::StyleVar_PinCorners, topRoundCornersFlags);
                            ed::BeginPin(pin.ID, ed::PinKind::Output);
                            ed::PinPivotRect(outputsRect.GetCenter(), outputsRect.GetCenter());
                            ed::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
                            ed::EndPin();
                            ed::PopStyleVar();


                            auto drawList = ImGui::GetWindowDrawList();
                            drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR(),
                                                    IM_COL32((int) (255 * pinBackground.x),
                                                             (int) (255 * pinBackground.y),
                                                             (int) (255 * pinBackground.z), outputAlpha), 4.0f,
                                                    allRoundCornersFlags);
                            drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR(),
                                              IM_COL32((int) (255 * pinBackground.x), (int) (255 * pinBackground.y),
                                                       (int) (255 * pinBackground.z), outputAlpha), 4.0f,
                                              allRoundCornersFlags);


                            if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
                                outputAlpha = (int) (255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
                        }

                        ImGui::EndHorizontal();
                    }

                    ImGui::EndVertical();

                    ed::EndNode();
                    ed::PopStyleVar(7);
                    ed::PopStyleColor(4);

                    // auto drawList = ed::GetNodeBackgroundDrawList(node.ID);

                    //const auto fringeScale = ImGui::GetStyle().AntiAliasFringeScale;
                    //const auto unitSize    = 1.0f / fringeScale;

                    //const auto ImDrawList_AddRect = [](ImDrawList* drawList, const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners, float thickness)
                    //{
                    //    if ((col >> 24) == 0)
                    //        return;
                    //    drawList->PathRect(a, b, rounding, rounding_corners);
                    //    drawList->PathStroke(col, true, thickness);
                    //};

                    //drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
                    //    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
                    //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
                    //drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
                    //    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
                    //ImGui::PopStyleVar();
                    //drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
                    //    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
                    ////ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
                    //drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
                    //    IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
                    ////ImGui::PopStyleVar();
                    //drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
                    //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
                    //drawList->AddRect(
                    //    contentRect.GetTL(),
                    //    contentRect.GetBR(),
                    //    IM_COL32(48, 128, 255, 100), 0.0f);
                    //ImGui::PopStyleVar();
                }

                for (auto &node: m_Nodes) {
                    if (node.Type != NodeType::Comment)
                        continue;

                    const float commentAlpha = 0.75f;

                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
                    ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
                    ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
                    ed::BeginNode(node.ID);
                    ImGui::PushID(node.ID.AsPointer());
                    ImGui::BeginVertical("content");
                    ImGui::BeginHorizontal("horizontal");
                    ImGui::Spring(1);
                    ImGui::TextUnformatted(node.Name.c_str());
                    ImGui::Spring(1);
                    ImGui::EndHorizontal();
                    ed::Group(node.Size);
                    ImGui::EndVertical();
                    ImGui::PopID();
                    ed::EndNode();
                    ed::PopStyleColor(2);
                    ImGui::PopStyleVar();

                    if (ed::BeginGroupHint(node.ID)) {
                        //auto alpha   = static_cast<int>(commentAlpha * ImGui::GetStyle().Alpha * 255);
                        auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

                        //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

                        auto min = ed::GetGroupMin();
                        //auto max = ed::GetGroupMax();

                        ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
                        ImGui::BeginGroup();
                        ImGui::TextUnformatted(node.Name.c_str());
                        ImGui::EndGroup();

                        auto drawList = ed::GetHintBackgroundDrawList();

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
                    ed::EndGroupHint();
                }

                for (auto &link: m_Links)
                    ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);

                if (!createNewNode) {
                    if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f)) {
                        auto showLabel = [](const char *label, ImColor color) {
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                            auto size = ImGui::CalcTextSize(label);

                            auto padding = ImGui::GetStyle().FramePadding;
                            auto spacing = ImGui::GetStyle().ItemSpacing;

                            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                            auto rectMin = ImGui::GetCursorScreenPos() - padding;
                            auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                            auto drawList = ImGui::GetWindowDrawList();
                            drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                            ImGui::TextUnformatted(label);
                        };

                        ed::PinId startPinId = 0, endPinId = 0;
                        if (ed::QueryNewLink(&startPinId, &endPinId)) {
                            auto startPin = FindPin(startPinId);
                            auto endPin = FindPin(endPinId);

                            newLinkPin = startPin ? startPin : endPin;

                            if (startPin->Kind == PinKind::Input) {
                                std::swap(startPin, endPin);
                                std::swap(startPinId, endPinId);
                            }

                            if (startPin && endPin) {
                                if (endPin == startPin) {
                                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                                } else if (endPin->Kind == startPin->Kind) {
                                    showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                                }
                                    //else if (endPin->Node == startPin->Node)
                                    //{
                                    //    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                                    //    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                                    //}
                                else if (endPin->Type != startPin->Type) {
                                    showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                                    ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                                } else {
                                    showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                                    if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                                        m_Links.emplace_back(Link(GetNextId(), startPinId, endPinId));
                                        m_Links.back().Color = GetIconColor(startPin->Type);
                                    }
                                }
                            }
                        }

                        ed::PinId pinId = 0;
                        if (ed::QueryNewNode(&pinId)) {
                            newLinkPin = FindPin(pinId);
                            if (newLinkPin)
                                showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                            if (ed::AcceptNewItem()) {
                                createNewNode = true;
                                newNodeLinkPin = FindPin(pinId);
                                newLinkPin = nullptr;
                                ed::Suspend();
                                ImGui::OpenPopup("Create New Node");
                                ed::Resume();
                            }
                        }
                    } else
                        newLinkPin = nullptr;

                    ed::EndCreate();

                    if (ed::BeginDelete()) {
                        ed::NodeId nodeId = 0;
                        while (ed::QueryDeletedNode(&nodeId)) {
                            if (ed::AcceptDeletedItem()) {
                                auto id = std::find_if(m_Nodes.begin(), m_Nodes.end(),
                                                       [nodeId](auto &node) { return node.ID == nodeId; });
                                if (id != m_Nodes.end())
                                    m_Nodes.erase(id);
                            }
                        }

                        ed::LinkId linkId = 0;
                        while (ed::QueryDeletedLink(&linkId)) {
                            if (ed::AcceptDeletedItem()) {
                                auto id = std::find_if(m_Links.begin(), m_Links.end(),
                                                       [linkId](auto &link) { return link.ID == linkId; });
                                if (id != m_Links.end())
                                    m_Links.erase(id);
                            }
                        }
                    }
                    ed::EndDelete();
                }

                ImGui::SetCursorScreenPos(cursorTopLeft);
            }

# if 1
            auto openPopupPosition = ImGui::GetMousePos();
            ed::Suspend();
            if (ed::ShowNodeContextMenu(&contextNodeId))
                ImGui::OpenPopup("Node Context Menu");
            else if (ed::ShowPinContextMenu(&contextPinId))
                ImGui::OpenPopup("Pin Context Menu");
            else if (ed::ShowLinkContextMenu(&contextLinkId))
                ImGui::OpenPopup("Link Context Menu");
            else if (ed::ShowBackgroundContextMenu()) {
                ImGui::OpenPopup("Create New Node");
                newNodeLinkPin = nullptr;
            }
            ed::Resume();

            ed::Suspend();
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
            if (ImGui::BeginPopup("Node Context Menu")) {
                auto node = FindNode(contextNodeId);

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
                    ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
                ImGui::Separator();
                if (ImGui::MenuItem("Delete"))
                    ed::DeleteNode(contextNodeId);
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopup("Pin Context Menu")) {
                auto pin = FindPin(contextPinId);

                ImGui::TextUnformatted("Pin Context Menu");
                ImGui::Separator();
                if (pin) {
                    ImGui::Text("ID: %p", pin->ID.AsPointer());
                    if (pin->Node)
                        ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
                    else
                        ImGui::Text("Node: %s", "<none>");
                } else
                    ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

                ImGui::EndPopup();
            }

            if (ImGui::BeginPopup("Link Context Menu")) {
                auto link = FindLink(contextLinkId);

                ImGui::TextUnformatted("Link Context Menu");
                ImGui::Separator();
                if (link) {
                    ImGui::Text("ID: %p", link->ID.AsPointer());
                    ImGui::Text("From: %p", link->StartPinID.AsPointer());
                    ImGui::Text("To: %p", link->EndPinID.AsPointer());
                } else
                    ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
                ImGui::Separator();
                if (ImGui::MenuItem("Delete"))
                    ed::DeleteLink(contextLinkId);
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopup("Create New Node")) {
                auto newNodePostion = openPopupPosition;
                //ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

                //auto drawList = ImGui::GetWindowDrawList();
                //drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

                Node *node = nullptr;
                if (ImGui::MenuItem("Input Action"))
                    node = blueprint->SpawnInputActionNode();
                if (ImGui::MenuItem("Output Action"))
                    node = blueprint->SpawnOutputActionNode();
                if (ImGui::MenuItem("Branch"))
                    node = blueprint->SpawnBranchNode();
                if (ImGui::MenuItem("Do N"))
                    node = blueprint->SpawnDoNNode();
                if (ImGui::MenuItem("Set Timer"))
                    node = blueprint->SpawnSetTimerNode();
                if (ImGui::MenuItem("Less"))
                    node = blueprint->SpawnLessNode();
                if (ImGui::MenuItem("Weird"))
                    node = blueprint->SpawnWeirdNode();
                if (ImGui::MenuItem("Trace by Channel"))
                    node = blueprint->SpawnTraceByChannelNode();
                if (ImGui::MenuItem("Print String"))
                    node = blueprint->SpawnPrintStringNode();
                ImGui::Separator();
                if (ImGui::MenuItem("Comment"))
                    node = blueprint->SpawnComment();
                ImGui::Separator();
                if (ImGui::MenuItem("Sequence"))
                    node = blueprint->SpawnTreeSequenceNode();
                if (ImGui::MenuItem("Move To"))
                    node = blueprint->SpawnTreeTaskNode();
                if (ImGui::MenuItem("Random Wait"))
                    node = blueprint->SpawnTreeTask2Node();
                ImGui::Separator();
                if (ImGui::MenuItem("Message"))
                    node = blueprint->SpawnMessageNode();
                ImGui::Separator();
                if (ImGui::MenuItem("Transform"))
                    node = blueprint->SpawnHoudiniTransformNode();
                if (ImGui::MenuItem("Group"))
                    node = blueprint->SpawnHoudiniGroupNode();

                if (node) {
                    blueprint->BuildNodes();

                    createNewNode = false;

                    ed::SetNodePosition(node->ID, newNodePostion);

                    if (auto startPin = newNodeLinkPin) {
                        auto &pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

                        for (auto &pin: pins) {
                            if (CanCreateLink(startPin, &pin)) {
                                auto endPin = &pin;
                                if (startPin->Kind == PinKind::Input)
                                    std::swap(startPin, endPin);

                                m_Links.emplace_back(Link(GetNextId(), startPin->ID, endPin->ID));
                                m_Links.back().Color = GetIconColor(startPin->Type);

                                break;
                            }
                        }
                    }
                }

                ImGui::EndPopup();
            } else
                createNewNode = false;
            ImGui::PopStyleVar();
            ed::Resume();
# endif


            /*
                cubic_bezier_t c;
                c.p0 = pointf(100, 600);
                c.p1 = pointf(300, 1200);
                c.p2 = pointf(500, 100);
                c.p3 = pointf(900, 600);

                auto drawList = ImGui::GetWindowDrawList();
                auto offset_radius = 15.0f;
                auto acceptPoint = [drawList, offset_radius](const bezier_subdivide_result_t& r)
                {
                    drawList->AddCircle(to_imvec(r.point), 4.0f, IM_COL32(255, 0, 255, 255));

                    auto nt = r.tangent.normalized();
                    nt = pointf(-nt.y, nt.x);

                    drawList->AddLine(to_imvec(r.point), to_imvec(r.point + nt * offset_radius), IM_COL32(255, 0, 0, 255), 1.0f);
                };

                drawList->AddBezierCurve(to_imvec(c.p0), to_imvec(c.p1), to_imvec(c.p2), to_imvec(c.p3), IM_COL32(255, 255, 255, 255), 1.0f);
                cubic_bezier_subdivide(acceptPoint, c);
            */

            ed::End();

            auto editorMin = ImGui::GetItemRectMin();
            auto editorMax = ImGui::GetItemRectMax();

            if (m_ShowOrdinals) {
                int nodeCount = ed::GetNodeCount();
                std::vector<ed::NodeId> orderedNodeIds;
                orderedNodeIds.resize(static_cast<size_t>(nodeCount));
                ed::GetOrderedNodeIds(orderedNodeIds.data(), nodeCount);


                auto drawList = ImGui::GetWindowDrawList();
                drawList->PushClipRect(editorMin, editorMax);

                int ordinal = 0;
                for (auto &nodeId: orderedNodeIds) {
                    auto p0 = ed::GetNodePosition(nodeId);
                    auto p1 = p0 + ed::GetNodeSize(nodeId);
                    p0 = ed::CanvasToScreen(p0);
                    p1 = ed::CanvasToScreen(p1);


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


            //ImGui::ShowTestWindow();
            //ImGui::ShowMetricsWindow();
        }

        ImGui::PopStyleVar(2);
        ImGui::End();
        ImGui::PopStyleVar(2);
    }
}