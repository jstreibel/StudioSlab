#include "ModelSemanticGraphArtistV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"
#include "Graphics/Utils.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <queue>
#include <set>

namespace Slab::Graphics::Plot2D::V2 {

    namespace ModelV2 = Slab::Core::Model::V2;
    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    namespace {

        constexpr DevFloat CHopRingSpacing = 5.4;
        constexpr DevFloat CLayoutOuterMargin = 2.3;
        constexpr DevFloat CPi = 3.14159265358979323846;
        constexpr DevFloat CEdgeHitTolerancePixels = 8.5;
        constexpr DevFloat CEdgeArrowLengthPixels = 13.0;
        constexpr DevFloat CEdgeArrowHalfWidthPixels = 4.8;
        constexpr DevFloat CEdgeNodeGapPixels = 2.5;
        constexpr DevFloat CHudMinPaddingX = 14.0;
        constexpr DevFloat CHudMinPaddingY = 12.0;
        constexpr DevFloat CHudMinSectionGap = 9.0;
        constexpr DevFloat CHudMinCardGap = 12.0;

        struct FHudCardMetricsV2 {
            DevFloat Width = 0.0;
            DevFloat Height = 0.0;
            DevFloat PaddingX = 0.0;
            DevFloat PaddingY = 0.0;
            DevFloat LineAdvance = 0.0;
            DevFloat SectionGap = 0.0;
            DevFloat CardGap = 0.0;
        };

        struct FDirectedEdgeGeometryV2 {
            FPoint2D LineStart{};
            FPoint2D LineEnd{};
            std::optional<std::array<FPoint2D, 3>> ArrowPoints{};
        };

        [[nodiscard]] auto KindRank(const ModelV2::ESemanticObjectKindV2 kind) -> int {
            switch (kind) {
                case ModelV2::ESemanticObjectKindV2::Definition: return 0;
                case ModelV2::ESemanticObjectKindV2::Relation: return 1;
                case ModelV2::ESemanticObjectKindV2::Assumption: return 2;
                case ModelV2::ESemanticObjectKindV2::VocabularyEntry: return 3;
            }

            return 0;
        }

        [[nodiscard]] auto CompactLabel(const Str &label, const std::size_t maxChars = 26) -> Str {
            if (label.size() <= maxChars) return label;
            if (maxChars <= 3) return label.substr(0, maxChars);
            return label.substr(0, maxChars - 3) + "...";
        }

        [[nodiscard]] auto ResolveHudFontHeight(const FPlotFrameContextV2 &frame) -> DevFloat {
            return std::max<DevFloat>(frame.TextMetrics.FontHeightPixels, 12.0);
        }

        [[nodiscard]] auto ResolveHudLineAdvance(const FPlotFrameContextV2 &frame) -> DevFloat {
            const auto fontHeight = ResolveHudFontHeight(frame);
            return std::max<DevFloat>(frame.TextMetrics.LineAdvancePixels, fontHeight);
        }

        [[nodiscard]] auto ResolveHudCharacterAdvance(const FPlotFrameContextV2 &frame) -> DevFloat {
            const auto fontHeight = ResolveHudFontHeight(frame);
            return std::max<DevFloat>(frame.TextMetrics.ApproxCharacterAdvancePixels, 0.5 * fontHeight);
        }

        [[nodiscard]] auto ResolveHudPaddingX(const FPlotFrameContextV2 &frame) -> DevFloat {
            return std::max<DevFloat>(CHudMinPaddingX, 0.75 * ResolveHudFontHeight(frame));
        }

        [[nodiscard]] auto ResolveHudPaddingY(const FPlotFrameContextV2 &frame) -> DevFloat {
            return std::max<DevFloat>(CHudMinPaddingY, 0.55 * ResolveHudLineAdvance(frame));
        }

        [[nodiscard]] auto ResolveHudSectionGap(const FPlotFrameContextV2 &frame) -> DevFloat {
            return std::max<DevFloat>(CHudMinSectionGap, 0.45 * ResolveHudLineAdvance(frame));
        }

        [[nodiscard]] auto ResolveHudCardGap(const FPlotFrameContextV2 &frame) -> DevFloat {
            return std::max<DevFloat>(CHudMinCardGap, 0.65 * ResolveHudFontHeight(frame));
        }

        [[nodiscard]] auto ApproximateHudTextWidth(const FPlotFrameContextV2 &frame, const Str &text) -> DevFloat {
            return static_cast<DevFloat>(text.size()) * ResolveHudCharacterAdvance(frame);
        }

        [[nodiscard]] auto BasePointSizeForKind(const ModelV2::ESemanticObjectKindV2 kind) -> DevFloat {
            switch (kind) {
                case ModelV2::ESemanticObjectKindV2::Definition: return 12.0;
                case ModelV2::ESemanticObjectKindV2::Relation: return 13.5;
                case ModelV2::ESemanticObjectKindV2::Assumption: return 11.5;
                case ModelV2::ESemanticObjectKindV2::VocabularyEntry: return 10.5;
            }

            return 11.0;
        }

        [[nodiscard]] auto BaseColorForNode(const FModelSemanticGraphArtistV2::FGraphNodeV2 &node) -> FColor {
            FColor color = LightGrey;
            switch (node.Kind) {
                case ModelV2::ESemanticObjectKindV2::Definition:
                    color = FColor::FromHex("#E2A33B");
                    break;
                case ModelV2::ESemanticObjectKindV2::Relation:
                    color = FColor::FromHex("#4FA7A2");
                    break;
                case ModelV2::ESemanticObjectKindV2::Assumption:
                    color = FColor::FromHex("#D56A4E");
                    break;
                case ModelV2::ESemanticObjectKindV2::VocabularyEntry:
                    color = FColor::FromHex("#5C8BCB");
                    break;
            }

            if (node.LayerRole == ModelV2::ESemanticGraphLayerRoleV2::Overlay) {
                return color.WithAlpha(0.84f);
            }

            return color;
        }

        [[nodiscard]] auto LabelColorForNode(const FModelSemanticGraphArtistV2::FGraphNodeV2 &node) -> FColor {
            auto color = node.bAmbient ? LightGrey : White;
            if (node.bConflict) color = FColor::FromHex("#F08473");
            if (node.bLocalOverride) color = FColor::FromHex("#F5D37A");
            if (node.LayerRole == ModelV2::ESemanticGraphLayerRoleV2::Overlay) {
                color = color.WithAlpha(0.8f);
            }
            return color;
        }

        [[nodiscard]] auto HaloColorForNode(const FModelSemanticGraphArtistV2::FGraphNodeV2 &node,
                                            const bool bSelected,
                                            const bool bHovered) -> FColor {
            if (bSelected) return FColor::FromHex("#FFF0A8").WithAlpha(0.34f);
            if (bHovered) return FColor::FromHex("#A8E6FF").WithAlpha(0.26f);
            if (node.bConflict) return FColor::FromHex("#F08473").WithAlpha(0.24f);
            if (node.bAmbient) return FColor::FromHex("#9DB7DA").WithAlpha(0.16f);
            if (node.LayerRole == ModelV2::ESemanticGraphLayerRoleV2::Overlay) {
                return BaseColorForNode(node).WithAlpha(0.1f);
            }
            return BaseColorForNode(node).WithAlpha(0.14f);
        }

        [[nodiscard]] auto EdgeColorForKind(const FModelSemanticGraphArtistV2::FGraphEdgeV2 &edge) -> FColor {
            if (edge.bConflict) return FColor::FromHex("#D66A5E");
            if (edge.bOverride) return FColor::FromHex("#F2C66D");

            switch (edge.Kind) {
                case FModelSemanticGraphArtistV2::EEdgeKind::Dependency:
                    return FColor::FromHex("#C6CDD8");
                case FModelSemanticGraphArtistV2::EEdgeKind::AmbientDependency:
                    return FColor::FromHex("#7DA8D8");
                case FModelSemanticGraphArtistV2::EEdgeKind::SourceLink:
                    return FColor::FromHex("#86C4B8");
                case FModelSemanticGraphArtistV2::EEdgeKind::TargetLink:
                    return FColor::FromHex("#8AC0C8");
                case FModelSemanticGraphArtistV2::EEdgeKind::Assumption:
                    return FColor::FromHex("#CC8A62");
                case FModelSemanticGraphArtistV2::EEdgeKind::Override:
                    return FColor::FromHex("#F2C66D");
            }

            return LightGrey;
        }

        [[nodiscard]] auto EdgeStyleForKind(const FModelSemanticGraphArtistV2::FGraphEdgeV2 &edge) -> PlotStyle {
            const auto alpha = edge.LayerRole == ModelV2::ESemanticGraphLayerRoleV2::Overlay
                ? (edge.bAmbient ? 0.56f : 0.66f)
                : (edge.bAmbient ? 0.72f : 0.84f);
            const auto thickness = edge.LayerRole == ModelV2::ESemanticGraphLayerRoleV2::Overlay
                ? (edge.bAmbient ? 0.9f : 1.05f)
                : (edge.bAmbient ? 1.0f : 1.35f);
            const auto color = EdgeColorForKind(edge).WithAlpha(alpha);
            auto style = PlotStyle(color, LineStrip, false, Nil, thickness);
            return style;
        }

        [[nodiscard]] auto IsDirectedEdge(const FModelSemanticGraphArtistV2::FGraphEdgeV2 &edge) -> bool {
            switch (edge.Kind) {
                case FModelSemanticGraphArtistV2::EEdgeKind::Dependency:
                case FModelSemanticGraphArtistV2::EEdgeKind::AmbientDependency:
                case FModelSemanticGraphArtistV2::EEdgeKind::SourceLink:
                case FModelSemanticGraphArtistV2::EEdgeKind::TargetLink:
                case FModelSemanticGraphArtistV2::EEdgeKind::Assumption:
                case FModelSemanticGraphArtistV2::EEdgeKind::Override:
                    return true;
            }

            return false;
        }

        [[nodiscard]] auto PixelVectorToPlotDelta(const FPoint2D &pixelVector,
                                                  const FPoint2D &pixelSize) -> FPoint2D {
            return {
                pixelVector.x * pixelSize.x,
                pixelVector.y * pixelSize.y
            };
        }

        [[nodiscard]] auto NodeEdgeInsetPixels(const FModelSemanticGraphArtistV2::FGraphNodeV2 &node) -> DevFloat {
            return std::max<DevFloat>(4.5, (0.55 * node.BasePointSize) + CEdgeNodeGapPixels);
        }

        [[nodiscard]] auto BuildDirectedEdgeGeometry(const FModelSemanticGraphArtistV2::FGraphNodeV2 &source,
                                                     const FModelSemanticGraphArtistV2::FGraphNodeV2 &target,
                                                     const FPoint2D &pixelSize,
                                                     const bool bDirected) -> FDirectedEdgeGeometryV2 {
            FDirectedEdgeGeometryV2 geometry{
                .LineStart = source.Position,
                .LineEnd = target.Position
            };

            if (pixelSize.x <= 0.0 || pixelSize.y <= 0.0) return geometry;

            const FPoint2D directionPixels{
                (target.Position.x - source.Position.x) / pixelSize.x,
                (target.Position.y - source.Position.y) / pixelSize.y
            };
            const auto lengthPixels = directionPixels.Length();
            if (lengthPixels <= 1.0e-6) return geometry;

            const auto unitPixels = (1.0 / lengthPixels) * directionPixels;
            const FPoint2D perpendicularPixels{-unitPixels.y, unitPixels.x};

            const auto sourceInsetPixels = NodeEdgeInsetPixels(source);
            const auto targetInsetPixels = NodeEdgeInsetPixels(target);
            geometry.LineStart = source.Position + PixelVectorToPlotDelta(sourceInsetPixels * unitPixels, pixelSize);

            const auto usableLengthPixels = lengthPixels - sourceInsetPixels - targetInsetPixels;
            if (usableLengthPixels <= 1.0) {
                geometry.LineEnd = geometry.LineStart;
                return geometry;
            }

            if (!bDirected) {
                geometry.LineEnd = target.Position - PixelVectorToPlotDelta(targetInsetPixels * unitPixels, pixelSize);
                return geometry;
            }

            const auto arrowLengthPixels = std::clamp<DevFloat>(0.45 * usableLengthPixels, 6.0, CEdgeArrowLengthPixels);
            const auto arrowHalfWidthPixels =
                std::clamp<DevFloat>(0.42 * arrowLengthPixels, 2.8, CEdgeArrowHalfWidthPixels);

            const auto tip = target.Position - PixelVectorToPlotDelta(targetInsetPixels * unitPixels, pixelSize);
            const auto arrowBaseCenter = tip - PixelVectorToPlotDelta(arrowLengthPixels * unitPixels, pixelSize);
            geometry.LineEnd = arrowBaseCenter;
            geometry.ArrowPoints = std::array<FPoint2D, 3>{
                arrowBaseCenter + PixelVectorToPlotDelta(arrowHalfWidthPixels * perpendicularPixels, pixelSize),
                tip,
                arrowBaseCenter - PixelVectorToPlotDelta(arrowHalfWidthPixels * perpendicularPixels, pixelSize)
            };

            return geometry;
        }

        [[nodiscard]] auto SemanticGraphLayerLine(const ModelV2::ESemanticGraphLayerRoleV2 layerRole,
                                                  const ModelV2::ESemanticGraphOverlayKindV2 overlayKind) -> Str {
            auto line = Str("Layer: ") + ModelV2::ToString(layerRole);
            if (overlayKind != ModelV2::ESemanticGraphOverlayKindV2::None) {
                line += " / ";
                line += ModelV2::ToString(overlayKind);
            }
            return line;
        }

        [[nodiscard]] auto DistanceToSegmentSquaredPixels(const FPoint2D &plotPosition,
                                                          const FPoint2D &a,
                                                          const FPoint2D &b,
                                                          const FPoint2D &pixelSize) -> DevFloat {
            const auto scaledAx = a.x / pixelSize.x;
            const auto scaledAy = a.y / pixelSize.y;
            const auto scaledBx = b.x / pixelSize.x;
            const auto scaledBy = b.y / pixelSize.y;
            const auto scaledPx = plotPosition.x / pixelSize.x;
            const auto scaledPy = plotPosition.y / pixelSize.y;

            const auto abx = scaledBx - scaledAx;
            const auto aby = scaledBy - scaledAy;
            const auto apx = scaledPx - scaledAx;
            const auto apy = scaledPy - scaledAy;

            const auto abLengthSquared = (abx * abx) + (aby * aby);
            if (abLengthSquared <= 1.0e-9) {
                return (apx * apx) + (apy * apy);
            }

            const auto t = std::clamp(
                ((apx * abx) + (apy * aby)) / abLengthSquared,
                static_cast<DevFloat>(0.0),
                static_cast<DevFloat>(1.0));
            const auto closestX = scaledAx + (t * abx);
            const auto closestY = scaledAy + (t * aby);
            const auto dx = scaledPx - closestX;
            const auto dy = scaledPy - closestY;
            return (dx * dx) + (dy * dy);
        }

        auto MeasureHudCard(const FPlotFrameContextV2 &frame,
                            const Str &title,
                            const Vector<Str> &lines) -> FHudCardMetricsV2 {
            const auto paddingX = ResolveHudPaddingX(frame);
            const auto paddingY = ResolveHudPaddingY(frame);
            const auto lineAdvance = ResolveHudLineAdvance(frame);
            const auto sectionGap = ResolveHudSectionGap(frame);

            DevFloat width = ApproximateHudTextWidth(frame, title);
            for (const auto &line : lines) {
                width = std::max(width, ApproximateHudTextWidth(frame, CompactLabel(line, 84)));
            }

            width += (2.0 * paddingX);
            const auto detailLineCount = static_cast<DevFloat>(lines.size());
            auto height = (2.0 * paddingY) + lineAdvance;
            if (detailLineCount > 0.0) {
                height += sectionGap + (detailLineCount * lineAdvance);
            }

            return {
                .Width = width,
                .Height = height,
                .PaddingX = paddingX,
                .PaddingY = paddingY,
                .LineAdvance = lineAdvance,
                .SectionGap = sectionGap,
                .CardGap = ResolveHudCardGap(frame)
            };
        }

        auto DrawHudCard(FPlotDrawListV2 &drawList,
                         const FHudCardMetricsV2 &metrics,
                         const DevFloat xMin,
                         const DevFloat yTop,
                         const Str &title,
                         const Vector<Str> &lines,
                         const FColor &accentColor) -> RectR {
            const RectR backgroundRect{
                xMin,
                xMin + metrics.Width,
                yTop - metrics.Height,
                yTop
            };
            drawList.AddRectangle(FRectangleCommandV2{
                .Rectangle = backgroundRect,
                .Color = FColor::FromHex("#0C1320").WithAlpha(0.86f),
                .bFilled = true,
                .CoordinateSpace = EPlotCoordinateSpaceV2::Screen
            });

            drawList.AddRectangle(FRectangleCommandV2{
                .Rectangle = backgroundRect,
                .Color = accentColor.WithAlpha(0.92f),
                .bFilled = false,
                .CoordinateSpace = EPlotCoordinateSpaceV2::Screen
            });

            const auto textX = xMin + metrics.PaddingX;
            auto textY = yTop - metrics.PaddingY - metrics.LineAdvance;
            drawList.AddText(FTextCommandV2{
                .Text = title,
                .Location = {textX, textY},
                .Color = accentColor,
                .CoordinateSpace = EPlotCoordinateSpaceV2::Screen
            });

            if (!lines.empty()) {
                textY -= metrics.SectionGap;
            }
            for (const auto &line : lines) {
                textY -= metrics.LineAdvance;
                drawList.AddText(FTextCommandV2{
                    .Text = CompactLabel(line, 84),
                    .Location = {textX, textY},
                    .Color = White.WithAlpha(0.95f),
                    .CoordinateSpace = EPlotCoordinateSpaceV2::Screen
                });
            }

            return backgroundRect;
        }

        auto AddHudCardFromTopRight(FPlotDrawListV2 &drawList,
                                    const FPlotFrameContextV2 &frame,
                                    const DevFloat xRight,
                                    const DevFloat yTop,
                                    const Str &title,
                                    const Vector<Str> &lines,
                                    const FColor &accentColor) -> DevFloat {
            if (title.empty() && lines.empty()) return yTop;

            const auto metrics = MeasureHudCard(frame, title, lines);
            const auto xMin = std::max(frame.HudLayout.SafeRect.xMin, xRight - metrics.Width);
            const auto rect = DrawHudCard(drawList, metrics, xMin, yTop, title, lines, accentColor);
            return rect.yMin - metrics.CardGap;
        }

        auto AddHudCardFromBottomLeft(FPlotDrawListV2 &drawList,
                                      const FPlotFrameContextV2 &frame,
                                      const DevFloat xMin,
                                      const DevFloat yBottom,
                                      const Str &title,
                                      const Vector<Str> &lines,
                                      const FColor &accentColor) -> DevFloat {
            if (title.empty() && lines.empty()) return yBottom;

            const auto metrics = MeasureHudCard(frame, title, lines);
            const auto clampedX = std::min(xMin, frame.HudLayout.SafeRect.xMax - metrics.Width);
            const auto rect = DrawHudCard(
                drawList,
                metrics,
                std::max(frame.HudLayout.SafeRect.xMin, clampedX),
                yBottom + metrics.Height,
                title,
                lines,
                accentColor);
            return rect.yMax + metrics.CardGap;
        }

    } // namespace

    FModelSemanticGraphArtistV2::FModelSemanticGraphArtistV2()
    : FPlotArtistV2("model_semantic_graph", "Model Semantic Graph") {
    }

    auto FModelSemanticGraphArtistV2::ApplyVisibilityFilters() -> void {
        Nodes.clear();
        Edges.clear();
        NodeIndexById.clear();
        HoveredNodeId.clear();
        HoveredEdgeId.clear();
        PressedNodeId.clear();

        if (!LastProjection.has_value()) return;

        const auto &projection = *LastProjection;
        if (projection.Nodes.empty()) {
            if (!projection.CenteredObject.IsValid()) {
                SelectedObject = {};
            }
            return;
        }

        if (!SelectedObject.IsValid()) {
            SelectedObject = projection.CenteredObject.IsValid() ? projection.CenteredObject : projection.Nodes.front().Ref;
        }

        const auto isLayerVisible = [this](const auto layerRole) {
            switch (layerRole) {
                case ModelV2::ESemanticGraphLayerRoleV2::Canonical: return bShowCanonicalLayer;
                case ModelV2::ESemanticGraphLayerRoleV2::Overlay: return bShowOverlayLayer;
            }

            return true;
        };

        std::set<Str> visibleNodeIds;
        for (const auto &graphNode : projection.Nodes) {
            if (!isLayerVisible(graphNode.LayerRole)) continue;
            visibleNodeIds.insert(graphNode.NodeId);
        }

        Nodes.reserve(visibleNodeIds.size());
        for (const auto &graphNode : projection.Nodes) {
            if (!visibleNodeIds.contains(graphNode.NodeId)) continue;

            FGraphNodeV2 node;
            node.Ref = graphNode.Ref;
            node.NodeId = graphNode.NodeId;
            node.Label = graphNode.Label;
            node.FullLabel = graphNode.FullLabel;
            node.Subtitle = graphNode.Subtitle;
            node.KindLabel = graphNode.KindLabel;
            node.CanonicalNotation = graphNode.CanonicalNotation;
            node.Description = graphNode.Description;
            node.Kind = graphNode.Kind;
            node.LayerRole = graphNode.LayerRole;
            node.OverlayKind = graphNode.OverlayKind;
            node.bReadonly = graphNode.bReadonly;
            node.bAmbient = graphNode.bAmbient;
            node.bConflict = graphNode.bConflict;
            node.bLocalOverride = graphNode.bLocalOverride;
            node.HopDistance = graphNode.HopDistance;
            node.BasePointSize = BasePointSizeForKind(graphNode.Kind);

            NodeIndexById[node.NodeId] = Nodes.size();
            Nodes.push_back(std::move(node));
        }

        Edges.reserve(projection.Edges.size());
        for (const auto &graphEdge : projection.Edges) {
            if (!isLayerVisible(graphEdge.LayerRole)) continue;
            if (!visibleNodeIds.contains(graphEdge.SourceNodeId) || !visibleNodeIds.contains(graphEdge.TargetNodeId)) continue;

            FGraphEdgeV2 edge;
            edge.EdgeId = graphEdge.EdgeId;
            edge.SourceNodeId = graphEdge.SourceNodeId;
            edge.TargetNodeId = graphEdge.TargetNodeId;
            edge.Kind = graphEdge.Kind;
            edge.LayerRole = graphEdge.LayerRole;
            edge.OverlayKind = graphEdge.OverlayKind;
            edge.Label = graphEdge.Label;
            edge.Detail = graphEdge.Detail;
            edge.bAmbient = graphEdge.bAmbient;
            edge.bConflict = graphEdge.bConflict;
            edge.bReadonly = graphEdge.bReadonly;
            edge.bOverride = graphEdge.bOverride;
            Edges.push_back(std::move(edge));
        }

        RebuildLayout();
    }

    auto FModelSemanticGraphArtistV2::SetSemanticGraphProjection(
        const ModelV2::FModelSemanticGraphProjectionV2 &projection) -> void {
        LastProjection = projection;
        SelectedObject = projection.CenteredObject;
        ApplyVisibilityFilters();
    }

    auto FModelSemanticGraphArtistV2::SetSemanticOverview(const ModelV2::FModelSemanticOverviewV2 &overview,
                                                          const ModelV2::FSemanticObjectRefV2 &selectedObject) -> void {
        LastOverview = overview;
        SetSemanticGraphProjection(ModelV2::BuildModelSemanticGraphProjectionV2(overview, selectedObject, NeighborhoodHops));
    }

    auto FModelSemanticGraphArtistV2::RebuildLayout() -> void {
        if (Nodes.empty()) return;

        std::map<int, Vector<std::size_t>> nodesByHop;
        for (std::size_t i = 0; i < Nodes.size(); ++i) {
            nodesByHop[Nodes[i].HopDistance].push_back(i);
        }

        for (auto &[hop, indices] : nodesByHop) {
            std::sort(indices.begin(), indices.end(), [&](const std::size_t lhs, const std::size_t rhs) {
                const auto &lhsNode = Nodes[lhs];
                const auto &rhsNode = Nodes[rhs];
                const auto lhsRank = KindRank(lhsNode.Kind);
                const auto rhsRank = KindRank(rhsNode.Kind);
                if (lhsRank != rhsRank) return lhsRank < rhsRank;
                if (lhsNode.Label != rhsNode.Label) return lhsNode.Label < rhsNode.Label;
                return lhsNode.NodeId < rhsNode.NodeId;
            });

            if (hop == 0) {
                Nodes[indices.front()].Position = {0.0, 0.0};
                continue;
            }

            const auto radius = CHopRingSpacing * static_cast<DevFloat>(hop);
            const auto count = static_cast<DevFloat>(std::max<std::size_t>(1, indices.size()));
            const auto angleOffset = (-0.5 * CPi) + (0.3 * static_cast<DevFloat>(hop));

            for (std::size_t slot = 0; slot < indices.size(); ++slot) {
                const auto angle = angleOffset + ((2.0 * CPi) * static_cast<DevFloat>(slot) / count);
                Nodes[indices[slot]].Position = {
                    radius * std::cos(angle),
                    radius * std::sin(angle)
                };
            }
        }
    }

    auto FModelSemanticGraphArtistV2::FindNodeById(const Str &nodeId) const -> const FGraphNodeV2 * {
        const auto it = NodeIndexById.find(nodeId);
        if (it == NodeIndexById.end()) return nullptr;
        if (it->second >= Nodes.size()) return nullptr;
        return &Nodes[it->second];
    }

    auto FModelSemanticGraphArtistV2::FindEdgeById(const Str &edgeId) const -> const FGraphEdgeV2 * {
        const auto it = std::find_if(Edges.begin(), Edges.end(), [&](const auto &edge) {
            return edge.EdgeId == edgeId;
        });
        if (it == Edges.end()) return nullptr;
        return &(*it);
    }

    auto FModelSemanticGraphArtistV2::FindEdgeHitId(const FPlotFrameContextV2 &frame,
                                                    const FPoint2D &plotPosition) const -> Str {
        if (Edges.empty()) return {};

        const auto pixelSize = PixelSizeInSpace(frame.PlotRegion, frame.Viewport);
        const auto toleranceSquared = CEdgeHitTolerancePixels * CEdgeHitTolerancePixels;

        const FGraphEdgeV2 *bestEdge = nullptr;
        DevFloat bestDistance = std::numeric_limits<DevFloat>::max();
        for (const auto &edge : Edges) {
            const auto *source = FindNodeById(edge.SourceNodeId);
            const auto *target = FindNodeById(edge.TargetNodeId);
            if (source == nullptr || target == nullptr) continue;

            const auto distance = DistanceToSegmentSquaredPixels(plotPosition, source->Position, target->Position, pixelSize);
            if (distance > toleranceSquared || distance >= bestDistance) continue;

            bestDistance = distance;
            bestEdge = &edge;
        }

        return bestEdge != nullptr ? bestEdge->EdgeId : Str{};
    }

    auto FModelSemanticGraphArtistV2::BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> {
        auto bindings = FPlotArtistV2::BuildReflectionParameterBindings();

        FPlotReflectionParameterBindingV2 nodeCountBinding;
        nodeCountBinding.Schema.ParameterId = "node_count";
        nodeCountBinding.Schema.DisplayName = "Node Count";
        nodeCountBinding.Schema.Description = "Number of semantic graph nodes currently projected.";
        nodeCountBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarInt32;
        nodeCountBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        nodeCountBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        nodeCountBinding.ReadCurrent = [this] {
            return ReflectionV2::MakeIntValue(static_cast<int>(Nodes.size()));
        };
        bindings.push_back(std::move(nodeCountBinding));

        FPlotReflectionParameterBindingV2 edgeCountBinding;
        edgeCountBinding.Schema.ParameterId = "edge_count";
        edgeCountBinding.Schema.DisplayName = "Edge Count";
        edgeCountBinding.Schema.Description = "Number of semantic graph edges currently projected.";
        edgeCountBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarInt32;
        edgeCountBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        edgeCountBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        edgeCountBinding.ReadCurrent = [this] {
            return ReflectionV2::MakeIntValue(static_cast<int>(Edges.size()));
        };
        bindings.push_back(std::move(edgeCountBinding));

        FPlotReflectionParameterBindingV2 selectedBinding;
        selectedBinding.Schema.ParameterId = "selected_object_id";
        selectedBinding.Schema.DisplayName = "Selected Object Id";
        selectedBinding.Schema.Description = "The semantic object currently centered by this graph artist.";
        selectedBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarString;
        selectedBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        selectedBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        selectedBinding.ReadCurrent = [this] { return ReflectionV2::MakeStringValue(SelectedObject.ObjectId); };
        bindings.push_back(std::move(selectedBinding));

        FPlotReflectionParameterBindingV2 hopsBinding;
        hopsBinding.Schema.ParameterId = "neighborhood_hops";
        hopsBinding.Schema.DisplayName = "Neighborhood Hops";
        hopsBinding.Schema.Description = "Radius of the projected semantic neighborhood around the selected object.";
        hopsBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarInt32;
        hopsBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        hopsBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        hopsBinding.Schema.Minimum = 1.0;
        hopsBinding.Schema.Maximum = 4.0;
        hopsBinding.ReadCurrent = [this] { return ReflectionV2::MakeIntValue(NeighborhoodHops); };
        hopsBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            try {
                SetNeighborhoodHops(std::stoi(value.Encoded));
            } catch (const std::exception &e) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_int",
                    "Could not parse int for parameter 'neighborhood_hops': " + Str(e.what()));
            }

            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(hopsBinding));

        FPlotReflectionParameterBindingV2 labelsBinding;
        labelsBinding.Schema.ParameterId = "show_labels";
        labelsBinding.Schema.DisplayName = "Show Labels";
        labelsBinding.Schema.Description = "Whether node labels are rendered.";
        labelsBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
        labelsBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        labelsBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        labelsBinding.ReadCurrent = [this] { return ReflectionV2::MakeBoolValue(bShowLabels); };
        labelsBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            bool parsed = false;
            if (!ReflectionV2::ParseBoolValue(value.Encoded, parsed)) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_bool",
                    "Could not parse bool for parameter 'show_labels'.");
            }

            bShowLabels = parsed;
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(labelsBinding));

        FPlotReflectionParameterBindingV2 canonicalLayerBinding;
        canonicalLayerBinding.Schema.ParameterId = "show_canonical_layer";
        canonicalLayerBinding.Schema.DisplayName = "Show Canonical";
        canonicalLayerBinding.Schema.Description = "Whether canonical model-semantic nodes and edges are visible.";
        canonicalLayerBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
        canonicalLayerBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        canonicalLayerBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        canonicalLayerBinding.ReadCurrent = [this] { return ReflectionV2::MakeBoolValue(bShowCanonicalLayer); };
        canonicalLayerBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            bool parsed = false;
            if (!ReflectionV2::ParseBoolValue(value.Encoded, parsed)) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_bool",
                    "Could not parse bool for parameter 'show_canonical_layer'.");
            }

            SetShowCanonicalLayer(parsed);
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(canonicalLayerBinding));

        FPlotReflectionParameterBindingV2 overlayLayerBinding;
        overlayLayerBinding.Schema.ParameterId = "show_overlay_layer";
        overlayLayerBinding.Schema.DisplayName = "Show Overlay";
        overlayLayerBinding.Schema.Description = "Whether overlay, provenance, and debug semantic nodes and edges are visible.";
        overlayLayerBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
        overlayLayerBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        overlayLayerBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        overlayLayerBinding.ReadCurrent = [this] { return ReflectionV2::MakeBoolValue(bShowOverlayLayer); };
        overlayLayerBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            bool parsed = false;
            if (!ReflectionV2::ParseBoolValue(value.Encoded, parsed)) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_bool",
                    "Could not parse bool for parameter 'show_overlay_layer'.");
            }

            SetShowOverlayLayer(parsed);
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(overlayLayerBinding));

        return bindings;
    }

    auto FModelSemanticGraphArtistV2::SetNeighborhoodHops(const int hops) -> void {
        NeighborhoodHops = std::clamp(hops, 1, 4);
        if (LastOverview.has_value()) {
            SetSemanticOverview(*LastOverview, SelectedObject);
        }
    }

    auto FModelSemanticGraphArtistV2::GetNeighborhoodHops() const -> int {
        return NeighborhoodHops;
    }

    auto FModelSemanticGraphArtistV2::SetShowLabels(const bool showLabels) -> void {
        bShowLabels = showLabels;
    }

    auto FModelSemanticGraphArtistV2::GetShowLabels() const -> bool {
        return bShowLabels;
    }

    auto FModelSemanticGraphArtistV2::SetShowCanonicalLayer(const bool showLayer) -> void {
        bShowCanonicalLayer = showLayer;
        ApplyVisibilityFilters();
    }

    auto FModelSemanticGraphArtistV2::GetShowCanonicalLayer() const -> bool {
        return bShowCanonicalLayer;
    }

    auto FModelSemanticGraphArtistV2::SetShowOverlayLayer(const bool showLayer) -> void {
        bShowOverlayLayer = showLayer;
        ApplyVisibilityFilters();
    }

    auto FModelSemanticGraphArtistV2::GetShowOverlayLayer() const -> bool {
        return bShowOverlayLayer;
    }

    auto FModelSemanticGraphArtistV2::SetOnSemanticObjectActivated(
        std::function<void(const ModelV2::FSemanticObjectRefV2 &)> callback) -> void {
        OnSemanticObjectActivated = std::move(callback);
    }

    auto FModelSemanticGraphArtistV2::GetNodes() const -> const Vector<FGraphNodeV2> & {
        return Nodes;
    }

    auto FModelSemanticGraphArtistV2::GetEdges() const -> const Vector<FGraphEdgeV2> & {
        return Edges;
    }

    auto FModelSemanticGraphArtistV2::EmitDrawCommands(const FPlotFrameContextV2 &frame,
                                                       FPlotDrawListV2 &drawList) const -> void {
        const auto pixelSize = PixelSizeInSpace(frame.PlotRegion, frame.Viewport);
        const auto labelOffsetX = std::max<DevFloat>(pixelSize.x * 8.0, 0.22);
        const auto labelOffsetY = std::max<DevFloat>(pixelSize.y * 10.0, 0.24);

        for (const auto &edge : Edges) {
            const auto *source = FindNodeById(edge.SourceNodeId);
            const auto *target = FindNodeById(edge.TargetNodeId);
            if (source == nullptr || target == nullptr) continue;

            const bool bTouchesSelection =
                ModelV2::AreSemanticObjectRefsEqualV2(source->Ref, SelectedObject) ||
                ModelV2::AreSemanticObjectRefsEqualV2(target->Ref, SelectedObject);
            const bool bHovered = edge.EdgeId == HoveredEdgeId;

            FPolylineCommandV2 command;
            command.Style = EdgeStyleForKind(edge);
            if (bTouchesSelection || bHovered) {
                auto highlightStyle = command.Style;
                highlightStyle.thickness = bHovered ? 2.7f : 2.05f;
                highlightStyle.lineColor = EdgeColorForKind(edge).WithAlpha(bHovered ? 1.0f : 0.96f);
                command.Style = highlightStyle;
            }
            const auto edgeGeometry = BuildDirectedEdgeGeometry(*source, *target, pixelSize, IsDirectedEdge(edge));
            const auto edgeStyle = command.Style;
            command.Points = {edgeGeometry.LineStart, edgeGeometry.LineEnd};
            drawList.AddPolyline(std::move(command));

            if (edgeGeometry.ArrowPoints.has_value()) {
                FPolylineCommandV2 arrowCommand;
                arrowCommand.Style = edgeStyle;
                arrowCommand.Points = {
                    (*edgeGeometry.ArrowPoints)[0],
                    (*edgeGeometry.ArrowPoints)[1],
                    (*edgeGeometry.ArrowPoints)[2]
                };
                drawList.AddPolyline(std::move(arrowCommand));
            }

            if (bHovered) {
                FTextCommandV2 edgeLabel;
                edgeLabel.Text = edge.Label;
                edgeLabel.Location = {
                    static_cast<DevFloat>(0.5 * (source->Position.x + target->Position.x)) + labelOffsetX,
                    static_cast<DevFloat>(0.5 * (source->Position.y + target->Position.y)) + labelOffsetY
                };
                edgeLabel.Color = EdgeColorForKind(edge).WithAlpha(0.96f);
                edgeLabel.CoordinateSpace = EPlotCoordinateSpaceV2::Plot;
                drawList.AddText(std::move(edgeLabel));
            }
        }

        for (const auto &node : Nodes) {
            const bool bSelected = ModelV2::AreSemanticObjectRefsEqualV2(node.Ref, SelectedObject);
            const bool bHovered = node.NodeId == HoveredNodeId;

            auto haloStyle = PlotStyle(
                HaloColorForNode(node, bSelected, bHovered),
                Points,
                false,
                Nil,
                static_cast<float>(node.BasePointSize + (bSelected ? 10.0 : (bHovered ? 6.0 : 2.5))));
            auto coreStyle = PlotStyle(
                BaseColorForNode(node),
                Points,
                false,
                Nil,
                static_cast<float>(node.BasePointSize + (bSelected ? 2.0 : (bHovered ? 1.0 : 0.0))));

            FPointSetCommandV2 haloCommand;
            haloCommand.Style = haloStyle;
            haloCommand.Points = {node.Position};
            drawList.AddPointSet(std::move(haloCommand));

            FPointSetCommandV2 coreCommand;
            coreCommand.Style = coreStyle;
            coreCommand.Points = {node.Position};
            drawList.AddPointSet(std::move(coreCommand));

            if (!bShowLabels) continue;

            FTextCommandV2 labelCommand;
            labelCommand.Text = node.Label;
            labelCommand.Location = node.Position.WithTranslation(labelOffsetX, labelOffsetY);
            labelCommand.Color = LabelColorForNode(node);
            labelCommand.CoordinateSpace = EPlotCoordinateSpaceV2::Plot;
            drawList.AddText(std::move(labelCommand));

            if (!node.Subtitle.empty() && (bSelected || bHovered)) {
                FTextCommandV2 subtitleCommand;
                subtitleCommand.Text = node.Subtitle;
                subtitleCommand.Location = node.Position.WithTranslation(labelOffsetX, -labelOffsetY);
                subtitleCommand.Color = LightGrey.WithAlpha(0.9f);
                subtitleCommand.CoordinateSpace = EPlotCoordinateSpaceV2::Plot;
                drawList.AddText(std::move(subtitleCommand));
            }
        }

        if (!Nodes.empty()) {
            DevFloat bottomY = frame.HudLayout.BottomLeft.y;

            const auto *selectedNode = [&]() -> const FGraphNodeV2 * {
                for (const auto &node : Nodes) {
                    if (ModelV2::AreSemanticObjectRefsEqualV2(node.Ref, SelectedObject)) return &node;
                }
                return nullptr;
            }();

            if (selectedNode != nullptr) {
                Vector<Str> lines;
                lines.push_back(selectedNode->FullLabel + " [" + selectedNode->KindLabel + "]");
                lines.push_back(SemanticGraphLayerLine(selectedNode->LayerRole, selectedNode->OverlayKind));
                if (!selectedNode->CanonicalNotation.empty()) lines.push_back(selectedNode->CanonicalNotation);
                else if (!selectedNode->Description.empty()) lines.push_back(selectedNode->Description);
                lines.push_back("Neighborhood hops: " + ToStr(NeighborhoodHops));
                bottomY = AddHudCardFromBottomLeft(
                    drawList,
                    frame,
                    frame.HudLayout.BottomLeft.x,
                    bottomY,
                    "Selected",
                    lines,
                    FColor::FromHex("#F2C66D"));
            }

            if (!HoveredNodeId.empty() && HoveredNodeId != (selectedNode != nullptr ? selectedNode->NodeId : Str{})) {
                if (const auto *hoveredNode = FindNodeById(HoveredNodeId); hoveredNode != nullptr) {
                    Vector<Str> lines;
                    lines.push_back(hoveredNode->FullLabel + " [" + hoveredNode->KindLabel + "]");
                    lines.push_back(SemanticGraphLayerLine(hoveredNode->LayerRole, hoveredNode->OverlayKind));
                    if (!hoveredNode->CanonicalNotation.empty()) lines.push_back(hoveredNode->CanonicalNotation);
                    else if (!hoveredNode->Description.empty()) lines.push_back(hoveredNode->Description);
                    if (!hoveredNode->Subtitle.empty()) lines.push_back(hoveredNode->Subtitle);
                    bottomY = AddHudCardFromBottomLeft(
                        drawList,
                        frame,
                        frame.HudLayout.BottomLeft.x,
                        bottomY,
                        "Hover",
                        lines,
                        FColor::FromHex("#8CD8F8"));
                }
            } else if (!HoveredEdgeId.empty()) {
                if (const auto *hoveredEdge = FindEdgeById(HoveredEdgeId); hoveredEdge != nullptr) {
                    const auto *source = FindNodeById(hoveredEdge->SourceNodeId);
                    const auto *target = FindNodeById(hoveredEdge->TargetNodeId);

                    Vector<Str> lines;
                    if (source != nullptr && target != nullptr) {
                        lines.push_back(source->FullLabel + " -> " + target->FullLabel);
                    }
                    lines.push_back(SemanticGraphLayerLine(hoveredEdge->LayerRole, hoveredEdge->OverlayKind));
                    if (!hoveredEdge->Detail.empty()) lines.push_back(hoveredEdge->Detail);
                    bottomY = AddHudCardFromBottomLeft(
                        drawList,
                        frame,
                        frame.HudLayout.BottomLeft.x,
                        bottomY,
                        "Hover Edge: " + hoveredEdge->Label,
                        lines,
                        EdgeColorForKind(*hoveredEdge));
                }
            }
        }

        if (!Nodes.empty() || LastProjection.has_value()) {
            DevFloat topRightY = frame.HudLayout.TopRight.y;
            Vector<Str> statusLines;
            const auto canonicalNodeCount = static_cast<std::size_t>(std::count_if(Nodes.begin(), Nodes.end(), [](const auto &node) {
                return node.LayerRole == ModelV2::ESemanticGraphLayerRoleV2::Canonical;
            }));
            const auto overlayNodeCount = Nodes.size() - canonicalNodeCount;
            const auto canonicalEdgeCount = static_cast<std::size_t>(std::count_if(Edges.begin(), Edges.end(), [](const auto &edge) {
                return edge.LayerRole == ModelV2::ESemanticGraphLayerRoleV2::Canonical;
            }));
            const auto overlayEdgeCount = Edges.size() - canonicalEdgeCount;
            statusLines.push_back(
                "Nodes: " + ToStr(Nodes.size()) +
                " (C " + ToStr(canonicalNodeCount) +
                " / O " + ToStr(overlayNodeCount) + ")");
            statusLines.push_back(
                "Edges: " + ToStr(Edges.size()) +
                " (C " + ToStr(canonicalEdgeCount) +
                " / O " + ToStr(overlayEdgeCount) + ")");
            statusLines.push_back(
                Str("Layers: C ") + (bShowCanonicalLayer ? "on" : "off") +
                " / O " + (bShowOverlayLayer ? "on" : "off"));
            if (Nodes.empty() && LastProjection.has_value() && !LastProjection->Nodes.empty()) {
                statusLines.push_back("No visible nodes for current layer filter.");
            }
            statusLines.push_back("Keys: [ ] radius, L labels, C canonical, O overlay, F fit");
            (void) AddHudCardFromTopRight(
                drawList,
                frame,
                frame.HudLayout.TopRight.x,
                topRightY,
                "Semantic Graph",
                statusLines,
                FColor::FromHex("#7DA8D8"));
        }
    }

    auto FModelSemanticGraphArtistV2::ComputePlotBounds() const -> std::optional<RectR> {
        if (Nodes.empty()) return std::nullopt;

        RectR bounds{
            Nodes.front().Position.x,
            Nodes.front().Position.x,
            Nodes.front().Position.y,
            Nodes.front().Position.y
        };

        for (const auto &node : Nodes) {
            bounds.xMin = std::min(bounds.xMin, node.Position.x);
            bounds.xMax = std::max(bounds.xMax, node.Position.x);
            bounds.yMin = std::min(bounds.yMin, node.Position.y);
            bounds.yMax = std::max(bounds.yMax, node.Position.y);
        }

        bounds.xMin -= CLayoutOuterMargin;
        bounds.xMax += CLayoutOuterMargin;
        bounds.yMin -= CLayoutOuterMargin;
        bounds.yMax += CLayoutOuterMargin;
        return bounds;
    }

    auto FModelSemanticGraphArtistV2::HitTest(const FPlotFrameContextV2 &frame,
                                              const FPoint2D &plotPosition,
                                              const FPoint2D &viewportPosition) const
        -> std::optional<FPlotHitTargetV2> {
        (void) viewportPosition;

        const auto pixelSize = PixelSizeInSpace(frame.PlotRegion, frame.Viewport);

        const FGraphNodeV2 *bestNode = nullptr;
        DevFloat bestDistance = std::numeric_limits<DevFloat>::max();
        for (const auto &node : Nodes) {
            const auto toleranceX = std::max<DevFloat>(pixelSize.x * (0.6 * node.BasePointSize), 0.18);
            const auto toleranceY = std::max<DevFloat>(pixelSize.y * (0.6 * node.BasePointSize), 0.18);
            const auto dx = (plotPosition.x - node.Position.x) / toleranceX;
            const auto dy = (plotPosition.y - node.Position.y) / toleranceY;
            const auto distance = (dx * dx) + (dy * dy);
            if (distance > 1.0 || distance >= bestDistance) continue;

            bestDistance = distance;
            bestNode = &node;
        }

        if (bestNode == nullptr) return std::nullopt;

        return FPlotHitTargetV2{
            .TargetId = bestNode->NodeId,
            .DisplayLabel = bestNode->Label,
            .Detail = bestNode->Subtitle
        };
    }

    auto FModelSemanticGraphArtistV2::HandlePointerEvent(const FPlotFrameContextV2 &frame,
                                                         const FPlotPointerEventV2 &event) -> bool {
        if (Nodes.empty()) return false;

        const auto hit = HitTest(frame, event.PlotPosition, event.ViewportPosition);
        const auto hitNodeId = hit.has_value() ? hit->TargetId : Str{};
        const auto hitEdgeId = hit.has_value() ? Str{} : FindEdgeHitId(frame, event.PlotPosition);

        switch (event.Kind) {
            case EPlotPointerEventKindV2::Move:
                HoveredNodeId = hitNodeId;
                HoveredEdgeId = hit.has_value() ? Str{} : hitEdgeId;
                return hit.has_value() || !HoveredEdgeId.empty();

            case EPlotPointerEventKindV2::Leave:
                HoveredNodeId.clear();
                HoveredEdgeId.clear();
                PressedNodeId.clear();
                return false;

            case EPlotPointerEventKindV2::Wheel:
                return false;

            case EPlotPointerEventKindV2::Button:
                if (event.Button != MouseButton_LEFT) return false;

                if (event.ButtonState == Press) {
                    PressedNodeId = hitNodeId;
                    HoveredNodeId = hitNodeId;
                    HoveredEdgeId.clear();
                    return hit.has_value();
                }

                if (event.ButtonState == Release) {
                    HoveredNodeId = hitNodeId;
                    HoveredEdgeId = hit.has_value() ? Str{} : hitEdgeId;
                    const bool bIsClick = !PressedNodeId.empty() && PressedNodeId == hitNodeId;
                    const auto *node = bIsClick ? FindNodeById(hitNodeId) : nullptr;
                    PressedNodeId.clear();

                    if (node != nullptr) {
                        if (OnSemanticObjectActivated) OnSemanticObjectActivated(node->Ref);
                        return true;
                    }
                }
                return false;
        }

        return false;
    }

    auto FModelSemanticGraphArtistV2::HandleKeyboardEvent(const FPlotFrameContextV2 &frame,
                                                          const FPlotKeyboardEventV2 &event) -> bool {
        (void) frame;

        if (event.State == Release) return false;

        switch (event.Key) {
            case Key_LEFT_BRACKET:
            case Key_MINUS:
            case Key_KP_SUBTRACT:
                SetNeighborhoodHops(NeighborhoodHops - 1);
                return true;

            case Key_RIGHT_BRACKET:
            case Key_EQUAL:
            case Key_PLUS:
            case Key_KP_ADD:
                SetNeighborhoodHops(NeighborhoodHops + 1);
                return true;

            case Key_L:
            case Key_l:
                SetShowLabels(!bShowLabels);
                return true;

            case Key_C:
            case Key_c:
                SetShowCanonicalLayer(!bShowCanonicalLayer);
                return true;

            case Key_O:
            case Key_o:
                SetShowOverlayLayer(!bShowOverlayLayer);
                return true;

            default:
                break;
        }

        return false;
    }

    auto FModelSemanticGraphArtistV2::GetArtistTypeId() const -> Str {
        return "model_semantic_graph";
    }

} // namespace Slab::Graphics::Plot2D::V2
