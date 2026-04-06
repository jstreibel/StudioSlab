#include "LabV2WindowManager.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>
#include <map>

namespace {

    auto TruncateCanvasLabel(const Slab::Str &label, const std::size_t maxChars = 56) -> Slab::Str {
        if (label.size() <= maxChars) return label;
        return label.substr(0, maxChars - 3) + "...";
    }

    auto DrawCanvasNodeBadge(ImDrawList *drawList, const ImVec2 position, const Slab::Str &text, const ImU32 color) -> float {
        if (drawList == nullptr || text.empty()) return 0.0f;

        const auto compactText = TruncateCanvasLabel(text, 14);
        const auto textSize = ImGui::CalcTextSize(compactText.c_str());
        const auto size = ImVec2(textSize.x + 12.0f, textSize.y + 4.0f);
        drawList->AddRectFilled(position, ImVec2(position.x + size.x, position.y + size.y), color, 5.0f);
        drawList->AddText(ImVec2(position.x + 6.0f, position.y + 2.0f), IM_COL32(246, 248, 252, 255), compactText.c_str());
        return size.x + 6.0f;
    }

    auto MeasureCanvasBadgeWidth(const Slab::Str &text) -> float {
        if (text.empty()) return 0.0f;
        const auto compactText = TruncateCanvasLabel(text, 14);
        const auto textSize = ImGui::CalcTextSize(compactText.c_str());
        return textSize.x + 12.0f + 6.0f;
    }

    [[nodiscard]] auto DistanceSquared(const ImVec2 a, const ImVec2 b) -> float {
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        return dx * dx + dy * dy;
    }

    [[nodiscard]] auto BezierPoint(const ImVec2 p0,
                                   const ImVec2 p1,
                                   const ImVec2 p2,
                                   const ImVec2 p3,
                                   const float t) -> ImVec2 {
        const float u = 1.0f - t;
        const float uu = u * u;
        const float tt = t * t;
        const float uuu = uu * u;
        const float ttt = tt * t;

        return ImVec2(
            uuu * p0.x + 3.0f * uu * t * p1.x + 3.0f * u * tt * p2.x + ttt * p3.x,
            uuu * p0.y + 3.0f * uu * t * p1.y + 3.0f * u * tt * p2.y + ttt * p3.y);
    }

    [[nodiscard]] auto DistanceSquaredToBezier(const ImVec2 p0,
                                               const ImVec2 p1,
                                               const ImVec2 p2,
                                               const ImVec2 p3,
                                               const ImVec2 point,
                                               const int segments = 24) -> float {
        float minDistance = std::numeric_limits<float>::max();
        ImVec2 previousPoint = p0;

        for (int i = 1; i <= segments; ++i) {
            const float t = static_cast<float>(i) / static_cast<float>(segments);
            const ImVec2 currentPoint = BezierPoint(p0, p1, p2, p3, t);

            const ImVec2 segment = ImVec2(currentPoint.x - previousPoint.x, currentPoint.y - previousPoint.y);
            const ImVec2 toPoint = ImVec2(point.x - previousPoint.x, point.y - previousPoint.y);
            const float segmentLengthSquared = segment.x * segment.x + segment.y * segment.y;
            float projection = 0.0f;
            if (segmentLengthSquared > 1.0e-6f) {
                projection = (toPoint.x * segment.x + toPoint.y * segment.y) / segmentLengthSquared;
                projection = std::clamp(projection, 0.0f, 1.0f);
            }
            const ImVec2 closestPoint = ImVec2(
                previousPoint.x + projection * segment.x,
                previousPoint.y + projection * segment.y);
            minDistance = std::min(minDistance, DistanceSquared(point, closestPoint));
            previousPoint = currentPoint;
        }

        return minDistance;
    }

    [[nodiscard]] auto ColorForGraphNodeKind(const Slab::Core::Reflection::V2::EGraphNodeKindV2 kind) -> std::pair<ImU32, ImU32> {
        using Slab::Core::Reflection::V2::EGraphNodeKindV2;

        switch (kind) {
            case EGraphNodeKindV2::Interface:
                return {IM_COL32(44, 40, 58, 238), IM_COL32(112, 94, 154, 255)};
            case EGraphNodeKindV2::Parameter:
                return {IM_COL32(38, 44, 55, 238), IM_COL32(92, 106, 132, 255)};
            case EGraphNodeKindV2::Operation:
                return {IM_COL32(34, 48, 46, 238), IM_COL32(78, 132, 120, 255)};
            case EGraphNodeKindV2::SemanticOperator:
                return {IM_COL32(34, 46, 56, 238), IM_COL32(86, 122, 152, 255)};
            case EGraphNodeKindV2::Coercion:
                return {IM_COL32(54, 46, 34, 238), IM_COL32(164, 132, 72, 255)};
            case EGraphNodeKindV2::RoutingEndpoint:
                return {IM_COL32(30, 52, 56, 238), IM_COL32(72, 146, 154, 255)};
            default:
                return {IM_COL32(40, 44, 52, 238), IM_COL32(96, 108, 126, 255)};
        }
    }

    [[nodiscard]] auto ColorForGraphEdgeKind(const Slab::Core::Reflection::V2::EGraphEdgeKindV2 kind) -> ImU32 {
        using Slab::Core::Reflection::V2::EGraphEdgeKindV2;

        switch (kind) {
            case EGraphEdgeKindV2::Containment:
                return IM_COL32(106, 172, 226, 148);
            case EGraphEdgeKindV2::ValueFlow:
                return IM_COL32(116, 214, 174, 148);
            case EGraphEdgeKindV2::SemanticCoercion:
                return IM_COL32(236, 194, 92, 180);
            case EGraphEdgeKindV2::HandleBinding:
                return IM_COL32(192, 154, 234, 168);
            case EGraphEdgeKindV2::StreamSubscription:
                return IM_COL32(126, 210, 236, 168);
            case EGraphEdgeKindV2::ControlDependency:
                return IM_COL32(226, 168, 106, 168);
            default:
                return IM_COL32(138, 146, 162, 132);
        }
    }

} // namespace

auto FLabV2WindowManager::DrawSubstrateGraphCanvasCommon(
    const char *canvasId,
    Slab::Core::Reflection::V2::FGraphDocumentV2 &document,
    Slab::Vector<FLabV2SubstrateGraphVisualNode> &nodes,
    float &bottomPanelHeight,
    const std::function<Slab::Vector<FLabV2SubstrateGraphCanvasAction>(const ImVec2 &)> &buildAddNodeActions,
    const std::function<Slab::Vector<FLabV2SubstrateGraphCanvasAction>(
        const Slab::Core::Reflection::V2::FGraphNodeV2 &,
        Slab::Core::Reflection::V2::FGraphPortV2 *,
        Slab::Core::Reflection::V2::EGraphPortDirectionV2,
        const ImVec2 &)> &buildConnectNodeActions,
    const std::function<void()> &markDirty,
    FLabV2SubstrateGraphCanvasInteraction *interaction) -> void {
    using namespace Slab::Core::Reflection::V2;

    if (interaction != nullptr) {
        interaction->ClickedNodeId.clear();
        interaction->ClickedEdgeId.clear();
        interaction->bBackgroundClicked = false;
    }

    constexpr float SplitterHeight = 6.0f;
    constexpr float BottomPanelMinHeight = 96.0f;
    constexpr float BottomPanelMaxHeight = 420.0f;
    constexpr float CanvasMinHeight = 220.0f;
    constexpr float CanvasLayoutEpsilon = 6.0f;

    static Slab::Str addPopupCanvasId;
    static ImVec2 addPopupWorldPos = ImVec2(0.0f, 0.0f);
    static bool bAddPopupRequestOpen = false;

    static bool bPinDragActive = false;
    static Slab::Str pinDragCanvasId;
    static Slab::Str pinDragNodeId;
    static Slab::Str pinDragPortId;
    static EGraphPortDirectionV2 pinDragDirection = EGraphPortDirectionV2::Output;
    static ImVec2 pinDragStartScreenPos = ImVec2(0.0f, 0.0f);

    static Slab::Str connectPopupCanvasId;
    static Slab::Str connectPopupNodeId;
    static Slab::Str connectPopupPortId;
    static EGraphPortDirectionV2 connectPopupDirection = EGraphPortDirectionV2::Output;
    static ImVec2 connectPopupWorldPos = ImVec2(0.0f, 0.0f);
    static bool bConnectPopupRequestOpen = false;
    static bool bRightDragActive = false;
    static Slab::Str rightDragCanvasId;
    static ImVec2 rightDragStartScreenPos = ImVec2(0.0f, 0.0f);

    const auto addPopupId = Slab::Str("##") + canvasId + ".add_node_popup";
    const auto connectPopupId = Slab::Str("##") + canvasId + ".connect_node_popup";

    const auto worldPointFromScreen = [](const ImVec2 &screenPos,
                                         const ImVec2 &canvasOrigin,
                                         const FGraphCanvasStateV2 &canvasState) -> ImVec2 {
        return ImVec2(
            screenPos.x - canvasOrigin.x - canvasState.PanX,
            screenPos.y - canvasOrigin.y - canvasState.PanY);
    };

    bottomPanelHeight = std::clamp(bottomPanelHeight, BottomPanelMinHeight, BottomPanelMaxHeight);

    auto canvasSize = ImGui::GetContentRegionAvail();
    if (canvasSize.x < 64.0f) canvasSize.x = 64.0f;
    const float canvasBottomReserve = bottomPanelHeight + SplitterHeight + CanvasLayoutEpsilon;
    canvasSize.y = std::max(CanvasMinHeight, canvasSize.y - canvasBottomReserve);

    const auto canvasPos = ImGui::GetCursorScreenPos();
    const auto canvasEnd = ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y);
    ImGui::InvisibleButton(canvasId, canvasSize, ImGuiButtonFlags_MouseButtonRight);
    ImGui::SetItemAllowOverlap();

    const bool bCanvasHovered = ImGui::IsItemHovered();
    const auto &io = ImGui::GetIO();
    const auto mousePos = io.MousePos;
    const bool bMouseInCanvas =
        mousePos.x >= canvasPos.x && mousePos.x <= canvasEnd.x &&
        mousePos.y >= canvasPos.y && mousePos.y <= canvasEnd.y;

    if (bCanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        bRightDragActive = true;
        rightDragCanvasId = canvasId;
        rightDragStartScreenPos = mousePos;
    }
    if (bRightDragActive && rightDragCanvasId == canvasId && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f)) {
            document.Canvas.PanX += io.MouseDelta.x;
            document.Canvas.PanY += io.MouseDelta.y;
            if (markDirty) markDirty();
        }
    }
    if (bRightDragActive && rightDragCanvasId == canvasId && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        constexpr float ClickDistanceSq = 4.0f * 4.0f;
        const bool bWasDragRelease = DistanceSquared(rightDragStartScreenPos, mousePos) > ClickDistanceSq;
        if (!bWasDragRelease && bMouseInCanvas) {
            addPopupCanvasId = canvasId;
            addPopupWorldPos = worldPointFromScreen(mousePos, canvasPos, document.Canvas);
            bAddPopupRequestOpen = true;
        }
        bRightDragActive = false;
        rightDragCanvasId.clear();
    } else if (!ImGui::IsMouseDown(ImGuiMouseButton_Right) && rightDragCanvasId == canvasId) {
        bRightDragActive = false;
        rightDragCanvasId.clear();
    }

    auto *drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(canvasPos, canvasEnd, IM_COL32(20, 24, 30, 255), 6.0f);
    drawList->AddRect(canvasPos, canvasEnd, IM_COL32(72, 82, 92, 255), 6.0f, 0, 1.0f);
    drawList->PushClipRect(canvasPos, canvasEnd, true);

    if (document.Canvas.bShowGrid) {
        constexpr float GridStep = 64.0f;
        const float xOffset = std::fmod(document.Canvas.PanX, GridStep);
        const float yOffset = std::fmod(document.Canvas.PanY, GridStep);
        for (float x = canvasPos.x + xOffset; x < canvasEnd.x; x += GridStep) {
            drawList->AddLine(ImVec2(x, canvasPos.y), ImVec2(x, canvasEnd.y), IM_COL32(36, 42, 52, 255), 1.0f);
        }
        for (float y = canvasPos.y + yOffset; y < canvasEnd.y; y += GridStep) {
            drawList->AddLine(ImVec2(canvasPos.x, y), ImVec2(canvasEnd.x, y), IM_COL32(36, 42, 52, 255), 1.0f);
        }
    }

    std::map<Slab::Str, FLabV2SubstrateGraphVisualNode *> visualNodeById;
    for (auto &visualNode : nodes) {
        if (visualNode.Node == nullptr) continue;
        visualNodeById[visualNode.Node->NodeId] = &visualNode;
    }

    const auto nodeScreenPos = [&](const FLabV2SubstrateGraphVisualNode &visualNode) -> ImVec2 {
        return ImVec2(
            canvasPos.x + document.Canvas.PanX + visualNode.Node->Position.x,
            canvasPos.y + document.Canvas.PanY + visualNode.Node->Position.y);
    };

    const auto pinPositionFor = [&](const FLabV2SubstrateGraphVisualNode &visualNode,
                                    const bool bOutput,
                                    const int slot,
                                    const int totalSlots) -> ImVec2 {
        const auto nodePos = nodeScreenPos(visualNode);
        const float slots = static_cast<float>(std::max(1, totalSlots));
        const float slotRatio = static_cast<float>(slot + 1) / (slots + 1.0f);
        const float y = nodePos.y + 52.0f + slotRatio * (visualNode.Size.y - 64.0f);
        const float x = bOutput ? nodePos.x + visualNode.Size.x : nodePos.x;
        return ImVec2(x, y);
    };

    const auto findPortSlot = [](const FGraphNodeV2 &node,
                                 const Slab::Str &portId,
                                 const EGraphPortDirectionV2 direction) -> std::pair<int, int> {
        int totalSlots = 0;
        int slotIndex = 0;
        bool bFound = false;
        for (const auto &port : node.Ports) {
            if (port.Direction != direction) continue;
            if (!bFound && port.PortId == portId) {
                slotIndex = totalSlots;
                bFound = true;
            }
            ++totalSlots;
        }
        totalSlots = std::max(1, totalSlots);
        if (!bFound) slotIndex = 0;
        return {slotIndex, totalSlots};
    };

    const auto portsForDirection = [](FGraphNodeV2 &node,
                                      const EGraphPortDirectionV2 direction) -> Slab::Vector<FGraphPortV2 *> {
        Slab::Vector<FGraphPortV2 *> ports;
        for (auto &port : node.Ports) {
            if (port.Direction != direction) continue;
            ports.push_back(&port);
        }
        return ports;
    };

    struct FRenderedEdge {
        const FGraphEdgeV2 *Edge = nullptr;
        ImVec2 P0 = ImVec2(0.0f, 0.0f);
        ImVec2 P1 = ImVec2(0.0f, 0.0f);
        ImVec2 P2 = ImVec2(0.0f, 0.0f);
        ImVec2 P3 = ImVec2(0.0f, 0.0f);
    };
    Slab::Vector<FRenderedEdge> renderedEdges;
    renderedEdges.reserve(document.Edges.size());

    for (const auto &edge : document.Edges) {
        const auto fromIt = visualNodeById.find(edge.FromNodeId);
        const auto toIt = visualNodeById.find(edge.ToNodeId);
        if (fromIt == visualNodeById.end() || toIt == visualNodeById.end()) continue;
        if (fromIt->second == nullptr || toIt->second == nullptr) continue;

        auto *fromVisual = fromIt->second;
        auto *toVisual = toIt->second;

        int fromSlot = 0;
        int fromSlots = std::max(1, fromVisual->OutputPins);
        int toSlot = 0;
        int toSlots = std::max(1, toVisual->InputPins);

        if (fromVisual->Node != nullptr && !fromVisual->Node->Ports.empty()) {
            const auto [slot, total] = findPortSlot(*fromVisual->Node, edge.FromPortId, EGraphPortDirectionV2::Output);
            fromSlot = slot;
            fromSlots = std::max(fromSlots, total);
        }
        if (toVisual->Node != nullptr && !toVisual->Node->Ports.empty()) {
            const auto [slot, total] = findPortSlot(*toVisual->Node, edge.ToPortId, EGraphPortDirectionV2::Input);
            toSlot = slot;
            toSlots = std::max(toSlots, total);
        }

        const auto p0 = pinPositionFor(*fromVisual, true, fromSlot, fromSlots);
        const auto p3 = pinPositionFor(*toVisual, false, toSlot, toSlots);
        const auto p1 = ImVec2(p0.x + 108.0f, p0.y);
        const auto p2 = ImVec2(p3.x - 108.0f, p3.y);
        drawList->AddBezierCubic(p0, p1, p2, p3, ColorForGraphEdgeKind(edge.Kind), 1.8f);

        renderedEdges.push_back(FRenderedEdge{.Edge = &edge, .P0 = p0, .P1 = p1, .P2 = p2, .P3 = p3});
    }

    bool bAnyNodeHovered = false;
    bool bAnyNodeClicked = false;
    bool bAnyPinHovered = false;
    bool bAnyPinClicked = false;

    for (auto &visualNode : nodes) {
        if (visualNode.Node == nullptr) continue;
        const auto nodePos = nodeScreenPos(visualNode);
        const auto nodeEnd = ImVec2(nodePos.x + visualNode.Size.x, nodePos.y + visualNode.Size.y);
        if (nodeEnd.x < canvasPos.x || nodePos.x > canvasEnd.x || nodeEnd.y < canvasPos.y || nodePos.y > canvasEnd.y) {
            continue;
        }

        ImGui::SetCursorScreenPos(nodePos);
        ImGui::InvisibleButton(
            (Slab::Str("##") + canvasId + ".node." + visualNode.Node->NodeId).c_str(),
            visualNode.Size,
            ImGuiButtonFlags_MouseButtonLeft);
        ImGui::SetItemAllowOverlap();
        const bool bNodeHovered = ImGui::IsItemHovered();
        bAnyNodeHovered = bAnyNodeHovered || bNodeHovered;

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            bAnyNodeClicked = true;
            if (interaction != nullptr) interaction->ClickedNodeId = visualNode.Node->NodeId;
        }

        const auto inputPins = std::max(0, visualNode.InputPins);
        const auto outputPins = std::max(0, visualNode.OutputPins);
        constexpr float PinCaptureRadiusSq = 9.0f * 9.0f;
        const bool bMouseOverNodePin = [&]() {
            for (int pin = 0; pin < inputPins; ++pin) {
                if (DistanceSquared(mousePos, pinPositionFor(visualNode, false, pin, inputPins)) <= PinCaptureRadiusSq) {
                    return true;
                }
            }
            for (int pin = 0; pin < outputPins; ++pin) {
                if (DistanceSquared(mousePos, pinPositionFor(visualNode, true, pin, outputPins)) <= PinCaptureRadiusSq) {
                    return true;
                }
            }
            return false;
        }();

        const bool bNodeDragSuppressedByPinDrag =
            bPinDragActive && pinDragCanvasId == canvasId && pinDragNodeId == visualNode.Node->NodeId;
        const bool bNodeDragSuppressedByPinHover =
            bMouseOverNodePin &&
            (ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Left));
        if (!bNodeDragSuppressedByPinDrag &&
            !bNodeDragSuppressedByPinHover &&
            ImGui::IsItemActive() &&
            ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
            visualNode.Node->Position.x += io.MouseDelta.x;
            visualNode.Node->Position.y += io.MouseDelta.y;
            if (markDirty) markDirty();
        }

        const auto [bodyColor, headerColor] = ColorForGraphNodeKind(visualNode.Node->Kind);
        const float lineHeight = ImGui::GetTextLineHeight();
        const float headerHeight = std::max(
            BlueprintNodeHeaderMinHeight,
            std::round(lineHeight * BlueprintNodeHeaderScale));
        drawList->AddRectFilled(nodePos, nodeEnd, bodyColor, 8.0f);
        drawList->AddRectFilled(nodePos, ImVec2(nodeEnd.x, nodePos.y + headerHeight), headerColor, 8.0f, ImDrawFlags_RoundCornersTop);
        drawList->AddRect(
            nodePos,
            nodeEnd,
            bNodeHovered ? IM_COL32(245, 195, 110, 255) : IM_COL32(96, 106, 122, 255),
            8.0f,
            0,
            bNodeHovered ? 2.0f : 1.0f);

        const float contentWidth = std::max(120.0f, visualNode.Size.x - 18.0f);
        const std::size_t titleChars = static_cast<std::size_t>(std::max(12.0f, std::floor(contentWidth / 8.3f)));
        const std::size_t subtitleChars = static_cast<std::size_t>(std::max(14.0f, std::floor(contentWidth / 8.0f)));
        drawList->AddText(
            ImVec2(nodePos.x + 9.0f, nodePos.y + 6.0f),
            IM_COL32(240, 244, 252, 255),
            TruncateCanvasLabel(visualNode.Title, titleChars).c_str());
        drawList->AddText(
            ImVec2(nodePos.x + 9.0f, nodePos.y + headerHeight + 6.0f),
            IM_COL32(182, 192, 208, 255),
            TruncateCanvasLabel(visualNode.Subtitle, subtitleChars).c_str());

        float badgeX = nodePos.x + 9.0f;
        float badgeY = std::max(nodePos.y + headerHeight + BlueprintNodeBadgeTopOffset, nodeEnd.y - 28.0f);
        for (const auto &badge : visualNode.Badges) {
            const float badgeWidth = MeasureCanvasBadgeWidth(badge.first);
            if (badgeX > nodePos.x + 9.0f && badgeX + badgeWidth > nodeEnd.x - 9.0f) {
                badgeX = nodePos.x + 9.0f;
                badgeY += ImGui::GetTextLineHeight() + 4.0f + BlueprintNodeBadgeRowGap;
            }
            if (badgeY > nodeEnd.y - 22.0f) break;
            badgeX += DrawCanvasNodeBadge(drawList, ImVec2(badgeX, badgeY), badge.first, badge.second);
        }

        auto inputPorts = portsForDirection(*visualNode.Node, EGraphPortDirectionV2::Input);
        auto outputPorts = portsForDirection(*visualNode.Node, EGraphPortDirectionV2::Output);

        for (int pin = 0; pin < inputPins; ++pin) {
            const auto pinPos = pinPositionFor(visualNode, false, pin, inputPins);
            drawList->AddCircleFilled(pinPos, 4.0f, IM_COL32(132, 220, 162, 255));

            FGraphPortV2 *port = (pin < static_cast<int>(inputPorts.size())) ? inputPorts[pin] : nullptr;
            const auto pinWidgetPos = ImVec2(pinPos.x - 7.0f, pinPos.y - 7.0f);
            ImGui::SetCursorScreenPos(pinWidgetPos);
            ImGui::InvisibleButton(
                (Slab::Str("##") + canvasId + ".pin.in." + visualNode.Node->NodeId + "." + Slab::ToStr(pin)).c_str(),
                ImVec2(14.0f, 14.0f),
                ImGuiButtonFlags_MouseButtonLeft);
            ImGui::SetItemAllowOverlap();

            const bool bPinHovered = ImGui::IsItemHovered();
            bAnyPinHovered = bAnyPinHovered || bPinHovered;
            if (bPinHovered) {
                const auto pinLabel =
                    port != nullptr
                        ? (port->DisplayName.empty() ? port->PortId : port->DisplayName)
                        : Slab::Str("Input ") + Slab::ToStr(pin + 1);
                ImGui::SetTooltip(
                    "%s\nId: %s\nDirection: Input%s%s",
                    pinLabel.c_str(),
                    port != nullptr ? port->PortId.c_str() : "<implicit>",
                    port != nullptr && !port->Handle.SpaceId.empty() ? "\nSpace: " : "",
                    port != nullptr && !port->Handle.SpaceId.empty() ? port->Handle.SpaceId.c_str() : "");
            }

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                bAnyPinClicked = true;
                bPinDragActive = true;
                pinDragCanvasId = canvasId;
                pinDragNodeId = visualNode.Node->NodeId;
                pinDragPortId = port != nullptr ? port->PortId : Slab::Str();
                pinDragDirection = EGraphPortDirectionV2::Input;
                pinDragStartScreenPos = pinPos;
            }
        }

        for (int pin = 0; pin < outputPins; ++pin) {
            const auto pinPos = pinPositionFor(visualNode, true, pin, outputPins);
            drawList->AddCircleFilled(pinPos, 4.0f, IM_COL32(138, 196, 246, 255));

            FGraphPortV2 *port = (pin < static_cast<int>(outputPorts.size())) ? outputPorts[pin] : nullptr;
            const auto pinWidgetPos = ImVec2(pinPos.x - 7.0f, pinPos.y - 7.0f);
            ImGui::SetCursorScreenPos(pinWidgetPos);
            ImGui::InvisibleButton(
                (Slab::Str("##") + canvasId + ".pin.out." + visualNode.Node->NodeId + "." + Slab::ToStr(pin)).c_str(),
                ImVec2(14.0f, 14.0f),
                ImGuiButtonFlags_MouseButtonLeft);
            ImGui::SetItemAllowOverlap();

            const bool bPinHovered = ImGui::IsItemHovered();
            bAnyPinHovered = bAnyPinHovered || bPinHovered;
            if (bPinHovered) {
                const auto pinLabel =
                    port != nullptr
                        ? (port->DisplayName.empty() ? port->PortId : port->DisplayName)
                        : Slab::Str("Output ") + Slab::ToStr(pin + 1);
                ImGui::SetTooltip(
                    "%s\nId: %s\nDirection: Output%s%s",
                    pinLabel.c_str(),
                    port != nullptr ? port->PortId.c_str() : "<implicit>",
                    port != nullptr && !port->Handle.SpaceId.empty() ? "\nSpace: " : "",
                    port != nullptr && !port->Handle.SpaceId.empty() ? port->Handle.SpaceId.c_str() : "");
            }

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                bAnyPinClicked = true;
                bPinDragActive = true;
                pinDragCanvasId = canvasId;
                pinDragNodeId = visualNode.Node->NodeId;
                pinDragPortId = port != nullptr ? port->PortId : Slab::Str();
                pinDragDirection = EGraphPortDirectionV2::Output;
                pinDragStartScreenPos = pinPos;
            }
        }
    }

    if (bPinDragActive && pinDragCanvasId == canvasId) {
        auto *sourceNode = static_cast<FGraphNodeV2 *>(nullptr);
        if (const auto sourceVisualIt = visualNodeById.find(pinDragNodeId); sourceVisualIt != visualNodeById.end()) {
            if (sourceVisualIt->second != nullptr && sourceVisualIt->second->Node != nullptr) {
                sourceNode = sourceVisualIt->second->Node;
            }
        }

        if (sourceNode == nullptr) {
            bPinDragActive = false;
        } else {
            const auto dragMousePos = io.MousePos;
            const float tangent =
                pinDragDirection == EGraphPortDirectionV2::Output
                    ? 84.0f
                    : -84.0f;
            drawList->AddBezierCubic(
                pinDragStartScreenPos,
                ImVec2(pinDragStartScreenPos.x + tangent, pinDragStartScreenPos.y),
                ImVec2(dragMousePos.x - tangent, dragMousePos.y),
                dragMousePos,
                IM_COL32(236, 194, 92, 220),
                2.0f);
            drawList->AddCircleFilled(dragMousePos, 4.0f, IM_COL32(246, 202, 116, 255));

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                bPinDragActive = false;
                const bool bDropInCanvas =
                    dragMousePos.x >= canvasPos.x && dragMousePos.x <= canvasEnd.x &&
                    dragMousePos.y >= canvasPos.y && dragMousePos.y <= canvasEnd.y;
                if (bDropInCanvas) {
                    connectPopupCanvasId = canvasId;
                    connectPopupNodeId = pinDragNodeId;
                    connectPopupPortId = pinDragPortId;
                    connectPopupDirection = pinDragDirection;
                    connectPopupWorldPos = worldPointFromScreen(dragMousePos, canvasPos, document.Canvas);
                    bConnectPopupRequestOpen = true;
                }
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                bPinDragActive = false;
            }
        }
    }

    if (interaction != nullptr && bCanvasHovered && bMouseInCanvas && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (bAnyPinClicked && !interaction->ClickedNodeId.empty()) {
            interaction->ClickedNodeId.clear();
        }

        if (!bAnyNodeHovered && !bAnyPinHovered && !bAnyNodeClicked && !bAnyPinClicked) {
            constexpr float EdgePickDistanceSq = 11.0f * 11.0f;
            const FRenderedEdge *pickedEdge = nullptr;
            float minDistanceSq = EdgePickDistanceSq;
            for (const auto &renderedEdge : renderedEdges) {
                const auto distanceSq = DistanceSquaredToBezier(
                    renderedEdge.P0,
                    renderedEdge.P1,
                    renderedEdge.P2,
                    renderedEdge.P3,
                    mousePos);
                if (distanceSq < minDistanceSq) {
                    minDistanceSq = distanceSq;
                    pickedEdge = &renderedEdge;
                }
            }

            if (pickedEdge != nullptr && pickedEdge->Edge != nullptr) {
                interaction->ClickedEdgeId = pickedEdge->Edge->EdgeId;
            } else {
                interaction->bBackgroundClicked = true;
            }
        }
    }

    drawList->PopClipRect();
    ImGui::SetCursorScreenPos(ImVec2(canvasPos.x, canvasEnd.y));

    if (bAddPopupRequestOpen && addPopupCanvasId == canvasId) {
        const auto screenPopupPos = ImVec2(
            canvasPos.x + document.Canvas.PanX + addPopupWorldPos.x,
            canvasPos.y + document.Canvas.PanY + addPopupWorldPos.y);
        ImGui::SetNextWindowPos(screenPopupPos, ImGuiCond_Appearing);
        ImGui::OpenPopup(addPopupId.c_str());
        bAddPopupRequestOpen = false;
    }
    if (ImGui::BeginPopup(addPopupId.c_str())) {
        auto actions = buildAddNodeActions ? buildAddNodeActions(addPopupWorldPos) : Slab::Vector<FLabV2SubstrateGraphCanvasAction>{};
        if (actions.empty()) {
            ImGui::TextDisabled("No addable nodes in this mode.");
        } else {
            for (const auto &action : actions) {
                if (action.bEnabled) {
                    if (ImGui::MenuItem(action.Label.c_str())) {
                        if (action.Execute) {
                            action.Execute();
                            if (markDirty) markDirty();
                        }
                        ImGui::CloseCurrentPopup();
                    }
                } else {
                    ImGui::BeginDisabled();
                    (void) ImGui::MenuItem(action.Label.c_str(), nullptr, false, false);
                    ImGui::EndDisabled();
                }
                if (ImGui::IsItemHovered() && !action.Hint.empty()) {
                    ImGui::SetTooltip("%s", action.Hint.c_str());
                }
            }
        }
        ImGui::EndPopup();
    }

    if (bConnectPopupRequestOpen && connectPopupCanvasId == canvasId) {
        const auto screenPopupPos = ImVec2(
            canvasPos.x + document.Canvas.PanX + connectPopupWorldPos.x,
            canvasPos.y + document.Canvas.PanY + connectPopupWorldPos.y);
        ImGui::SetNextWindowPos(screenPopupPos, ImGuiCond_Appearing);
        ImGui::OpenPopup(connectPopupId.c_str());
        bConnectPopupRequestOpen = false;
    }
    if (ImGui::BeginPopup(connectPopupId.c_str())) {
        auto *sourceNode = static_cast<FGraphNodeV2 *>(nullptr);
        for (auto &node : document.Nodes) {
            if (node.NodeId != connectPopupNodeId) continue;
            sourceNode = &node;
            break;
        }

        FGraphPortV2 *sourcePort = nullptr;
        if (sourceNode != nullptr && !connectPopupPortId.empty()) {
            for (auto &port : sourceNode->Ports) {
                if (port.PortId != connectPopupPortId) continue;
                if (port.Direction != connectPopupDirection) continue;
                sourcePort = &port;
                break;
            }
        }

        if (sourceNode == nullptr) {
            ImGui::TextDisabled("Source node no longer exists.");
        } else {
            auto actions = buildConnectNodeActions
                ? buildConnectNodeActions(*sourceNode, sourcePort, connectPopupDirection, connectPopupWorldPos)
                : Slab::Vector<FLabV2SubstrateGraphCanvasAction>{};
            if (actions.empty()) {
                ImGui::TextDisabled("No connectable nodes suggested.");
            } else {
                for (const auto &action : actions) {
                    if (action.bEnabled) {
                        if (ImGui::MenuItem(action.Label.c_str())) {
                            if (action.Execute) {
                                action.Execute();
                                if (markDirty) markDirty();
                            }
                            ImGui::CloseCurrentPopup();
                        }
                    } else {
                        ImGui::BeginDisabled();
                        (void) ImGui::MenuItem(action.Label.c_str(), nullptr, false, false);
                        ImGui::EndDisabled();
                    }
                    if (ImGui::IsItemHovered() && !action.Hint.empty()) {
                        ImGui::SetTooltip("%s", action.Hint.c_str());
                    }
                }
            }
        }
        ImGui::EndPopup();
    }

    const auto splitterId = Slab::Str("##") + canvasId + ".splitter";
    ImGui::InvisibleButton(splitterId.c_str(), ImVec2(canvasSize.x, SplitterHeight), ImGuiButtonFlags_MouseButtonLeft);
    const bool bSplitterHovered = ImGui::IsItemHovered();
    const bool bSplitterActive = ImGui::IsItemActive();
    if (bSplitterHovered || bSplitterActive) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    }
    if (bSplitterActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
        const auto &dragIo = ImGui::GetIO();
        const float previousHeight = bottomPanelHeight;
        bottomPanelHeight = std::clamp(
            bottomPanelHeight - dragIo.MouseDelta.y,
            BottomPanelMinHeight,
            BottomPanelMaxHeight);
        if (std::abs(bottomPanelHeight - previousHeight) > 1.0e-4f) {
            if (markDirty) markDirty();
        }
    }

    const auto splitterRectMin = ImGui::GetItemRectMin();
    const auto splitterRectMax = ImGui::GetItemRectMax();
    const float splitterCenterY = 0.5f * (splitterRectMin.y + splitterRectMax.y);
    drawList->AddLine(
        ImVec2(splitterRectMin.x + 2.0f, splitterCenterY),
        ImVec2(splitterRectMax.x - 2.0f, splitterCenterY),
        bSplitterActive ? IM_COL32(246, 202, 116, 210) : IM_COL32(96, 108, 124, 192),
        bSplitterActive ? 2.0f : 1.0f);
}
