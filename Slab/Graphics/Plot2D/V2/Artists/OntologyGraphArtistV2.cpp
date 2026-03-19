#include "OntologyGraphArtistV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"
#include "Graphics/Utils.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <set>

namespace Slab::Graphics::Plot2D::V2 {

    namespace OntologyV2 = Slab::Core::Ontology::V2;
    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    namespace {

        constexpr DevFloat CNodeMinWidthPixels = 144.0;
        constexpr DevFloat CNodeMinHeightPixels = 72.0;
        constexpr DevFloat CNodeMinPaddingXPixels = 14.0;
        constexpr DevFloat CNodeMinPaddingYPixels = 11.0;
        constexpr DevFloat CNodeMinSecondaryGapPixels = 8.0;
        constexpr DevFloat CNodeMinStatusBadgeWidthPixels = 72.0;
        constexpr DevFloat CNodeMinStatusBadgeHeightPixels = 20.0;
        constexpr DevFloat CNodeMinStudyBadgeWidthPixels = 44.0;
        constexpr DevFloat CNodeMinStudyBadgeHeightPixels = 18.0;
        constexpr DevFloat CNodeMinOwnershipBadgeWidthPixels = 46.0;
        constexpr DevFloat CNodeMinOwnershipBadgeHeightPixels = 18.0;
        constexpr DevFloat CNodeBoundsMinHalfWidth = 6.4;
        constexpr DevFloat CNodeBoundsMinHalfHeight = 4.2;
        constexpr DevFloat CBoundsPadding = 4.2;
        constexpr DevFloat CFitPaddingFraction = 0.08;
        constexpr DevFloat CNodeMinFontScale = 0.60;
        constexpr DevFloat CNodeMaxFontScale = 2.80;
        constexpr DevFloat CEdgeHitTolerancePixels = 8.0;
        constexpr DevFloat CEdgeArrowLengthPixels = 12.0;
        constexpr DevFloat CEdgeArrowHalfWidthPixels = 4.5;
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

        struct FEdgeRouteV2 {
            Vector<FPoint2D> Points;
            std::optional<std::array<FPoint2D, 3>> ArrowPoints;
            FPoint2D LabelPoint{};
        };

        [[nodiscard]] auto CompactLabel(const Str &label, const std::size_t maxChars = 34) -> Str {
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
            return std::max<DevFloat>(frame.TextMetrics.ApproxCharacterAdvancePixels, 0.52 * fontHeight);
        }

        [[nodiscard]] auto ResolveNodeFontHeight(const FPlotFrameContextV2 &frame,
                                                 const DevFloat fontScale = 1.0) -> DevFloat {
            const auto baseFontHeight = std::max<DevFloat>(frame.TextMetrics.FontHeightPixels, 12.0);
            return std::max<DevFloat>(1.0, fontScale) * baseFontHeight;
        }

        [[nodiscard]] auto ResolveNodeLineAdvance(const FPlotFrameContextV2 &frame,
                                                  const DevFloat fontScale = 1.0) -> DevFloat {
            const auto baseFontHeight = std::max<DevFloat>(frame.TextMetrics.FontHeightPixels, 12.0);
            const auto baseLineAdvance = std::max<DevFloat>(frame.TextMetrics.LineAdvancePixels, 1.18 * baseFontHeight);
            return std::max<DevFloat>(1.0, fontScale) * baseLineAdvance;
        }

        [[nodiscard]] auto ResolveNodeCharacterAdvance(const FPlotFrameContextV2 &frame,
                                                       const DevFloat fontScale = 1.0) -> DevFloat {
            const auto baseFontHeight = std::max<DevFloat>(frame.TextMetrics.FontHeightPixels, 12.0);
            const auto baseAdvance = std::max<DevFloat>(frame.TextMetrics.ApproxCharacterAdvancePixels, 0.56 * baseFontHeight);
            return std::max<DevFloat>(1.0, fontScale) * baseAdvance;
        }

        [[nodiscard]] auto ResolveNodePaddingX(const FPlotFrameContextV2 &frame,
                                               const DevFloat fontScale = 1.0) -> DevFloat {
            return std::max<DevFloat>(CNodeMinPaddingXPixels, 0.92 * ResolveNodeFontHeight(frame, fontScale));
        }

        [[nodiscard]] auto ResolveNodePaddingY(const FPlotFrameContextV2 &frame,
                                               const DevFloat fontScale = 1.0) -> DevFloat {
            return std::max<DevFloat>(CNodeMinPaddingYPixels, 0.62 * ResolveNodeLineAdvance(frame, fontScale));
        }

        [[nodiscard]] auto ResolveNodeSecondaryGap(const FPlotFrameContextV2 &frame,
                                                   const DevFloat fontScale = 1.0) -> DevFloat {
            return std::max<DevFloat>(CNodeMinSecondaryGapPixels, 0.36 * ResolveNodeLineAdvance(frame, fontScale));
        }

        [[nodiscard]] auto ResolveNodeMinWidth(const FPlotFrameContextV2 &frame,
                                               const DevFloat fontScale = 1.0) -> DevFloat {
            return std::max<DevFloat>(CNodeMinWidthPixels, 9.0 * ResolveNodeFontHeight(frame, fontScale));
        }

        [[nodiscard]] auto ResolveNodeMaxWidth(const FPlotFrameContextV2 &frame,
                                               const DevFloat fontScale = 1.0) -> DevFloat {
            return std::max<DevFloat>(340.0, 23.0 * ResolveNodeFontHeight(frame, fontScale));
        }

        [[nodiscard]] auto ApproximateNodeTextWidth(const FPlotFrameContextV2 &frame,
                                                    const Str &text,
                                                    const std::size_t maxChars,
                                                    const DevFloat fontScale = 1.0) -> DevFloat {
            return static_cast<DevFloat>(std::min(maxChars, std::max<std::size_t>(4, text.size()))) *
                ResolveNodeCharacterAdvance(frame, fontScale);
        }

        [[nodiscard]] auto ResolveStatusBadgeWidth(const FPlotFrameContextV2 &frame,
                                                   const Str &label,
                                                   const DevFloat fontScale = 1.0) -> DevFloat {
            return std::max<DevFloat>(
                CNodeMinStatusBadgeWidthPixels,
                ApproximateNodeTextWidth(frame, label, 14, fontScale) + (1.55 * ResolveNodePaddingX(frame, fontScale)));
        }

        [[nodiscard]] auto ResolveStatusBadgeHeight(const FPlotFrameContextV2 &frame,
                                                    const DevFloat fontScale = 1.0) -> DevFloat {
            return std::max<DevFloat>(CNodeMinStatusBadgeHeightPixels, 0.98 * ResolveNodeLineAdvance(frame, fontScale));
        }

        [[nodiscard]] auto ResolveStudyBadgeWidth(const FPlotFrameContextV2 &frame,
                                                  const DevFloat fontScale = 1.0) -> DevFloat {
            return std::max<DevFloat>(
                CNodeMinStudyBadgeWidthPixels,
                ApproximateNodeTextWidth(frame, "study", 10, fontScale) + (1.2 * ResolveNodePaddingX(frame, fontScale)));
        }

        [[nodiscard]] auto ResolveStudyBadgeHeight(const FPlotFrameContextV2 &frame,
                                                   const DevFloat fontScale = 1.0) -> DevFloat {
            return std::max<DevFloat>(CNodeMinStudyBadgeHeightPixels, 0.92 * ResolveNodeLineAdvance(frame, fontScale));
        }

        [[nodiscard]] auto OwnershipBadgeLabel(const OntologyV2::FOntologyProjectedNodeV2 &node) -> Str {
            return node.OwnershipScope == OntologyV2::EOntologyOwnershipScopeV2::Global ? "global" : "local";
        }

        [[nodiscard]] auto ResolveOwnershipBadgeWidth(const FPlotFrameContextV2 &frame,
                                                      const Str &label,
                                                      const DevFloat fontScale = 1.0) -> DevFloat {
            return std::max<DevFloat>(
                CNodeMinOwnershipBadgeWidthPixels,
                ApproximateNodeTextWidth(frame, label, 10, fontScale) + (1.15 * ResolveNodePaddingX(frame, fontScale)));
        }

        [[nodiscard]] auto ResolveOwnershipBadgeHeight(const FPlotFrameContextV2 &frame,
                                                       const DevFloat fontScale = 1.0) -> DevFloat {
            return std::max<DevFloat>(CNodeMinOwnershipBadgeHeightPixels, 0.88 * ResolveNodeLineAdvance(frame, fontScale));
        }

        [[nodiscard]] auto EstimateNodeBoundsHalfWidth(const OntologyV2::FOntologyProjectedNodeV2 &node) -> DevFloat {
            const auto footerText = node.SecondarySummary.empty() ? node.CompactId : node.SecondarySummary;
            const auto dominantChars = static_cast<DevFloat>(std::max(
                std::clamp<std::size_t>(node.Title.size(), 8, 40),
                std::clamp<std::size_t>(footerText.size(), 6, 44)));
            const auto badgeAllowance = node.bStudyRoot ? 1.15 : 0.95;
            return std::clamp(
                CNodeBoundsMinHalfWidth + (0.19 * dominantChars) + badgeAllowance,
                CNodeBoundsMinHalfWidth,
                15.8);
        }

        [[nodiscard]] auto EstimateNodeBoundsHalfHeight(const OntologyV2::FOntologyProjectedNodeV2 &node) -> DevFloat {
            auto halfHeight = CNodeBoundsMinHalfHeight;
            if (!node.SecondarySummary.empty() || !node.CompactId.empty()) {
                halfHeight += 1.2;
            }
            if (node.bStudyRoot) {
                halfHeight += 0.45;
            }
            return halfHeight;
        }

        [[nodiscard]] auto ComputeProjectionBounds(const OntologyV2::FOntologyGraphProjection &projection)
            -> std::optional<RectR> {
            if (projection.Nodes.empty()) return std::nullopt;

            RectR bounds{
                projection.Nodes.front().Position.x,
                projection.Nodes.front().Position.x,
                projection.Nodes.front().Position.y,
                projection.Nodes.front().Position.y
            };

            for (const auto &node : projection.Nodes) {
                const auto halfWidth = EstimateNodeBoundsHalfWidth(node);
                const auto halfHeight = EstimateNodeBoundsHalfHeight(node);
                bounds.xMin = std::min(bounds.xMin, node.Position.x - halfWidth);
                bounds.xMax = std::max(bounds.xMax, node.Position.x + halfWidth);
                bounds.yMin = std::min(bounds.yMin, node.Position.y - halfHeight);
                bounds.yMax = std::max(bounds.yMax, node.Position.y + halfHeight);
            }

            bounds.xMin -= CBoundsPadding;
            bounds.xMax += CBoundsPadding;
            bounds.yMin -= CBoundsPadding;
            bounds.yMax += CBoundsPadding;
            return bounds;
        }

        [[nodiscard]] auto ResolveOntologyTextScale(const FPlotFrameContextV2 &frame,
                                                    const OntologyV2::FOntologyGraphProjection &projection) -> DevFloat {
            const auto referenceBounds = ComputeProjectionBounds(projection);
            if (!referenceBounds.has_value()) return 1.0;

            auto fittedBounds = *referenceBounds;
            const auto padX = CFitPaddingFraction * fittedBounds.GetWidth();
            const auto padY = CFitPaddingFraction * fittedBounds.GetHeight();
            fittedBounds.xMin -= padX;
            fittedBounds.xMax += padX;
            fittedBounds.yMin -= padY;
            fittedBounds.yMax += padY;

            const auto regionWidth = std::max<DevFloat>(frame.PlotRegion.GetWidth(), 1.0e-6);
            const auto regionHeight = std::max<DevFloat>(frame.PlotRegion.GetHeight(), 1.0e-6);
            const auto scaleX = fittedBounds.GetWidth() / regionWidth;
            const auto scaleY = fittedBounds.GetHeight() / regionHeight;
            const auto scale = std::max(scaleX, scaleY);

            if (!std::isfinite(scale)) return 1.0;
            return std::clamp(scale, CNodeMinFontScale, CNodeMaxFontScale);
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

        [[nodiscard]] auto MeasureHudCard(const FPlotFrameContextV2 &frame,
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
            auto height = (2.0 * paddingY) + lineAdvance;
            if (!lines.empty()) {
                height += sectionGap + (static_cast<DevFloat>(lines.size()) * lineAdvance);
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
            const RectR backgroundRect{xMin, xMin + metrics.Width, yTop - metrics.Height, yTop};

            drawList.AddRectangle(FRectangleCommandV2{
                .Rectangle = backgroundRect,
                .Color = FColor::FromHex("#0C1320").WithAlpha(0.88f),
                .bFilled = true,
                .CoordinateSpace = EPlotCoordinateSpaceV2::Screen
            });

            drawList.AddRectangle(FRectangleCommandV2{
                .Rectangle = backgroundRect,
                .Color = accentColor.WithAlpha(0.96f),
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
                    .Color = White.WithAlpha(0.96f),
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

        [[nodiscard]] auto MixColor(const FColor &lhs, const FColor &rhs, const DevFloat t) -> FColor {
            const auto a = std::clamp(t, static_cast<DevFloat>(0.0), static_cast<DevFloat>(1.0));
            return {
                static_cast<float>((1.0 - a) * lhs.r + a * rhs.r),
                static_cast<float>((1.0 - a) * lhs.g + a * rhs.g),
                static_cast<float>((1.0 - a) * lhs.b + a * rhs.b),
                static_cast<float>((1.0 - a) * lhs.a + a * rhs.a)
            };
        }

        [[nodiscard]] auto LayerColor(const Str &layer) -> FColor {
            if (layer == "ambient") return FColor::FromHex("#6C7483");
            if (layer == "semantic") return FColor::FromHex("#4A92C2");
            if (layer == "descent") return FColor::FromHex("#D59A3B");
            if (layer == "realization") return FColor::FromHex("#3E9D95");
            if (layer == "recipe") return FColor::FromHex("#6BA85D");
            if (layer == "artifact") return FColor::FromHex("#B36F5A");
            if (layer == "study") return FColor::FromHex("#C9C36D");
            return LightGrey;
        }

        [[nodiscard]] auto TypeColor(const Str &type) -> FColor {
            if (type == "classifies_as" || type == "instance_of") return FColor::FromHex("#8EB8DA");
            if (type == "can_descend_to" || type == "admits_realization" || type == "admits_solver") return FColor::FromHex("#DAAA63");
            if (type == "produces" || type == "stepped_by" || type == "discretized_by") return FColor::FromHex("#8ACA8C");
            if (type == "activates") return FColor::FromHex("#68C3E0");
            if (type == "satisfies") return FColor::FromHex("#6FCE91");
            if (type == "blocks") return FColor::FromHex("#E46F64");
            if (type == "requires" || type == "requires_for_descent" || type == "depends_on") return FColor::FromHex("#C9A27A");
            return FColor::FromHex("#A6B0C1");
        }

        [[nodiscard]] auto StatusAccentColor(const OntologyV2::EOntologyActivationStatusV2 status) -> FColor {
            switch (status) {
                case OntologyV2::EOntologyActivationStatusV2::Active: return FColor::FromHex("#64C7EC");
                case OntologyV2::EOntologyActivationStatusV2::Reachable: return FColor::FromHex("#E4B85C");
                case OntologyV2::EOntologyActivationStatusV2::Satisfied: return FColor::FromHex("#71D28F");
                case OntologyV2::EOntologyActivationStatusV2::Blocked: return FColor::FromHex("#EA6C62");
                case OntologyV2::EOntologyActivationStatusV2::Unresolved: return FColor::FromHex("#AAB4C4");
                case OntologyV2::EOntologyActivationStatusV2::Partial: return FColor::FromHex("#D9A15B");
                case OntologyV2::EOntologyActivationStatusV2::None: return LightGrey;
            }

            return LightGrey;
        }

        [[nodiscard]] auto SelectionEquals(const OntologyV2::FOntologyGraphSelectionV2 &lhs,
                                           const OntologyV2::FOntologyGraphSelectionV2 &rhs) -> bool {
            return lhs.Kind == rhs.Kind && lhs.ElementId == rhs.ElementId;
        }

        [[nodiscard]] auto PixelVectorToPlotDelta(const FPoint2D &pixelVector,
                                                  const FPoint2D &pixelSize) -> FPoint2D {
            return {pixelVector.x * pixelSize.x, pixelVector.y * pixelSize.y};
        }

        [[nodiscard]] auto MeasureNodeRect(const FPlotFrameContextV2 &frame,
                                           const OntologyV2::FOntologyProjectedNodeV2 &node,
                                           const DevFloat fontScale) -> RectR {
            const auto pixelSize = PixelSizeInSpace(frame.PlotRegion, frame.Viewport);
            const auto paddingX = ResolveNodePaddingX(frame, fontScale);
            const auto paddingY = ResolveNodePaddingY(frame, fontScale);
            const auto lineAdvance = ResolveNodeLineAdvance(frame, fontScale);
            const auto secondaryGap = ResolveNodeSecondaryGap(frame, fontScale);

            const auto secondaryText = node.SecondarySummary.empty() ? node.CompactId : node.SecondarySummary;
            const auto kindText = CompactLabel(node.Kind, 18);
            const auto statusText = CompactLabel(OntologyV2::ToString(node.ActivationStatus), 14);
            const auto titleWidthPixels = ApproximateNodeTextWidth(frame, node.Title, 38, fontScale);
            const auto footerLeftWidthPixels = ApproximateNodeTextWidth(frame, secondaryText, 40, fontScale);
            const auto footerRightWidthPixels = ApproximateNodeTextWidth(frame, kindText, 18, fontScale);
            const auto leadingBadgeReservePixels = node.bStudyRoot
                ? ResolveStudyBadgeWidth(frame, fontScale) + (0.55 * paddingX)
                : ResolveOwnershipBadgeWidth(frame, OwnershipBadgeLabel(node), fontScale) + (0.55 * paddingX);
            const auto statusBadgeReservePixels = node.ActivationStatus != OntologyV2::EOntologyActivationStatusV2::None
                ? ResolveStatusBadgeWidth(frame, statusText, fontScale)
                : std::max<DevFloat>(24.0, 1.45 * ResolveNodeFontHeight(frame, fontScale));
            const auto widthPixels = std::clamp(
                std::max(
                    titleWidthPixels + leadingBadgeReservePixels,
                    footerLeftWidthPixels + footerRightWidthPixels + (1.25 * paddingX)) +
                    (2.0 * paddingX) +
                    statusBadgeReservePixels,
                ResolveNodeMinWidth(frame, fontScale),
                ResolveNodeMaxWidth(frame, fontScale));
            auto heightPixels = std::max<DevFloat>(
                CNodeMinHeightPixels,
                (2.0 * paddingY) + (2.0 * lineAdvance) + secondaryGap + (0.55 * ResolveNodeFontHeight(frame, fontScale)));
            if (!node.SecondarySummary.empty() || !node.CompactId.empty()) {
                heightPixels = std::max<DevFloat>(
                    heightPixels,
                    (2.0 * paddingY) + (2.0 * lineAdvance) + secondaryGap + (0.8 * ResolveNodeFontHeight(frame, fontScale)));
            }
            if (node.bStudyRoot) {
                heightPixels = std::max<DevFloat>(heightPixels, CNodeMinHeightPixels + (0.45 * lineAdvance));
            }

            const auto halfWidth = 0.5 * widthPixels * pixelSize.x;
            const auto halfHeight = 0.5 * heightPixels * pixelSize.y;
            return {
                node.Position.x - halfWidth,
                node.Position.x + halfWidth,
                node.Position.y - halfHeight,
                node.Position.y + halfHeight
            };
        }

        [[nodiscard]] auto BuildRectOutlinePoints(const RectR &rect) -> Vector<FPoint2D> {
            return {
                {rect.xMin, rect.yMin},
                {rect.xMin, rect.yMax},
                {rect.xMax, rect.yMax},
                {rect.xMax, rect.yMin}
            };
        }

        [[nodiscard]] auto BuildEdgeRoute(const RectR &sourceRect,
                                          const RectR &targetRect,
                                          const FPoint2D &pixelSize) -> FEdgeRouteV2 {
            const auto sourceCenter = FPoint2D{0.5 * (sourceRect.xMin + sourceRect.xMax), 0.5 * (sourceRect.yMin + sourceRect.yMax)};
            const auto targetCenter = FPoint2D{0.5 * (targetRect.xMin + targetRect.xMax), 0.5 * (targetRect.yMin + targetRect.yMax)};

            const bool bToRight = targetCenter.x >= sourceCenter.x;
            const auto start = FPoint2D{bToRight ? sourceRect.xMax : sourceRect.xMin, sourceCenter.y};
            const auto end = FPoint2D{bToRight ? targetRect.xMin : targetRect.xMax, targetCenter.y};
            const auto midX = 0.5 * (start.x + end.x);

            FEdgeRouteV2 route;
            if (std::abs(start.y - end.y) < (pixelSize.y * 8.0)) {
                route.Points = {start, end};
            } else {
                route.Points = {start, {midX, start.y}, {midX, end.y}, end};
            }

            route.LabelPoint = route.Points.size() > 2
                ? FPoint2D{midX, 0.5 * (start.y + end.y)}
                : FPoint2D{0.5 * (start.x + end.x), 0.5 * (start.y + end.y)};

            const auto &arrowStart = route.Points[route.Points.size() - 2];
            const auto &arrowTip = route.Points.back();
            if (pixelSize.x <= 0.0 || pixelSize.y <= 0.0) return route;

            const FPoint2D directionPixels{
                (arrowTip.x - arrowStart.x) / pixelSize.x,
                (arrowTip.y - arrowStart.y) / pixelSize.y
            };
            const auto lengthPixels = directionPixels.Length();
            if (lengthPixels <= 1.0e-6) return route;

            const auto unitPixels = (1.0 / lengthPixels) * directionPixels;
            const FPoint2D perpendicularPixels{-unitPixels.y, unitPixels.x};
            const auto arrowBase = arrowTip - PixelVectorToPlotDelta(CEdgeArrowLengthPixels * unitPixels, pixelSize);
            route.ArrowPoints = std::array<FPoint2D, 3>{
                arrowBase + PixelVectorToPlotDelta(CEdgeArrowHalfWidthPixels * perpendicularPixels, pixelSize),
                arrowTip,
                arrowBase - PixelVectorToPlotDelta(CEdgeArrowHalfWidthPixels * perpendicularPixels, pixelSize)
            };
            return route;
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

        [[nodiscard]] auto EdgeBadgeLabel(const Str &type) -> Str {
            if (type == "classifies_as") return "classifies";
            if (type == "can_descend_to") return "descend";
            if (type == "requires_for_descent") return "requires";
            if (type == "admits_realization") return "realizes";
            if (type == "admits_solver") return "solver";
            if (type == "stepped_by") return "schedule";
            if (type == "produces") return "artifact";
            if (type == "depends_on") return "depends";
            if (type == "activates") return "active";
            return type;
        }

        auto BuildHighlightSets(const OntologyV2::FOntologyGraphProjection &projection,
                                const OntologyV2::FOntologyGraphSelectionV2 &selection,
                                std::set<Str> &highlightedNodeIds,
                                std::set<Str> &highlightedEdgeIds) -> void {
            if (!selection.IsValid()) return;

            if (selection.Kind == OntologyV2::EOntologyElementKindV2::Node) {
                highlightedNodeIds.insert(selection.ElementId);
                const auto outgoingIt = projection.OutgoingEdgesByNodeId.find(selection.ElementId);
                if (outgoingIt != projection.OutgoingEdgesByNodeId.end()) {
                    for (const auto &edgeId : outgoingIt->second) {
                        highlightedEdgeIds.insert(edgeId);
                        if (const auto *edge = projection.FindEdge(edgeId); edge != nullptr) {
                            highlightedNodeIds.insert(edge->ToNodeId);
                        }
                    }
                }

                const auto incomingIt = projection.IncomingEdgesByNodeId.find(selection.ElementId);
                if (incomingIt != projection.IncomingEdgesByNodeId.end()) {
                    for (const auto &edgeId : incomingIt->second) {
                        highlightedEdgeIds.insert(edgeId);
                        if (const auto *edge = projection.FindEdge(edgeId); edge != nullptr) {
                            highlightedNodeIds.insert(edge->FromNodeId);
                        }
                    }
                }
            } else if (selection.Kind == OntologyV2::EOntologyElementKindV2::Edge) {
                highlightedEdgeIds.insert(selection.ElementId);
                if (const auto *edge = projection.FindEdge(selection.ElementId); edge != nullptr) {
                    highlightedNodeIds.insert(edge->FromNodeId);
                    highlightedNodeIds.insert(edge->ToNodeId);
                }
            }
        }

    } // namespace

    FOntologyGraphArtistV2::FOntologyGraphArtistV2()
    : FPlotArtistV2("ontology_graph", "Ontology Graph") {
    }

    auto FOntologyGraphArtistV2::RebuildIndexes() -> void {
        NodeIndexById.clear();
        EdgeIndexById.clear();
        for (std::size_t index = 0; index < Projection.Nodes.size(); ++index) {
            NodeIndexById[Projection.Nodes[index].NodeId] = index;
        }
        for (std::size_t index = 0; index < Projection.Edges.size(); ++index) {
            EdgeIndexById[Projection.Edges[index].EdgeId] = index;
        }
    }

    auto FOntologyGraphArtistV2::NormalizeSelection() -> void {
        const auto selectionExists = [&](const OntologyV2::FOntologyGraphSelectionV2 &selection) {
            if (!selection.IsValid()) return false;
            if (selection.Kind == OntologyV2::EOntologyElementKindV2::Node) {
                return NodeIndexById.contains(selection.ElementId);
            }
            if (selection.Kind == OntologyV2::EOntologyElementKindV2::Edge) {
                return EdgeIndexById.contains(selection.ElementId);
            }
            return false;
        };

        if (!selectionExists(SelectedSelection)) {
            SelectedSelection = {};
        }
        if (!selectionExists(HoveredSelection)) {
            HoveredSelection = {};
        }
        if (!selectionExists(PressedSelection)) {
            PressedSelection = {};
        }

        if (!SelectedSelection.IsValid() && !Projection.SelectedStudyId.empty() && NodeIndexById.contains(Projection.SelectedStudyId)) {
            SelectedSelection = {
                OntologyV2::EOntologyElementKindV2::Node,
                Projection.SelectedStudyId
            };
        }
    }

    auto FOntologyGraphArtistV2::FindNode(const Str &nodeId) const
        -> const OntologyV2::FOntologyProjectedNodeV2 * {
        const auto it = NodeIndexById.find(nodeId);
        if (it == NodeIndexById.end() || it->second >= Projection.Nodes.size()) return nullptr;
        return &Projection.Nodes[it->second];
    }

    auto FOntologyGraphArtistV2::FindEdge(const Str &edgeId) const
        -> const OntologyV2::FOntologyProjectedEdgeV2 * {
        const auto it = EdgeIndexById.find(edgeId);
        if (it == EdgeIndexById.end() || it->second >= Projection.Edges.size()) return nullptr;
        return &Projection.Edges[it->second];
    }

    auto FOntologyGraphArtistV2::BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> {
        auto bindings = FPlotArtistV2::BuildReflectionParameterBindings();

        FPlotReflectionParameterBindingV2 nodeCountBinding;
        nodeCountBinding.Schema.ParameterId = "node_count";
        nodeCountBinding.Schema.DisplayName = "Node Count";
        nodeCountBinding.Schema.Description = "Number of ontology nodes currently visible.";
        nodeCountBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarInt32;
        nodeCountBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        nodeCountBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        nodeCountBinding.ReadCurrent = [this] {
            return ReflectionV2::MakeIntValue(static_cast<int>(Projection.Nodes.size()));
        };
        bindings.push_back(std::move(nodeCountBinding));

        FPlotReflectionParameterBindingV2 edgeCountBinding;
        edgeCountBinding.Schema.ParameterId = "edge_count";
        edgeCountBinding.Schema.DisplayName = "Edge Count";
        edgeCountBinding.Schema.Description = "Number of ontology edges currently visible.";
        edgeCountBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarInt32;
        edgeCountBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        edgeCountBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        edgeCountBinding.ReadCurrent = [this] {
            return ReflectionV2::MakeIntValue(static_cast<int>(Projection.Edges.size()));
        };
        bindings.push_back(std::move(edgeCountBinding));

        FPlotReflectionParameterBindingV2 selectionBinding;
        selectionBinding.Schema.ParameterId = "selected_element_id";
        selectionBinding.Schema.DisplayName = "Selected Element";
        selectionBinding.Schema.Description = "Stable id of the selected node or edge.";
        selectionBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarString;
        selectionBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        selectionBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        selectionBinding.ReadCurrent = [this] {
            return ReflectionV2::MakeStringValue(SelectedSelection.ElementId);
        };
        bindings.push_back(std::move(selectionBinding));

        FPlotReflectionParameterBindingV2 labelsBinding;
        labelsBinding.Schema.ParameterId = "show_edge_labels";
        labelsBinding.Schema.DisplayName = "Show Edge Labels";
        labelsBinding.Schema.Description = "Whether compact edge badges are drawn.";
        labelsBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
        labelsBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        labelsBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        labelsBinding.ReadCurrent = [this] {
            return ReflectionV2::MakeBoolValue(bShowEdgeLabels);
        };
        labelsBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            bool parsed = false;
            if (!ReflectionV2::ParseBoolValue(value.Encoded, parsed)) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_bool",
                    "Could not parse bool for parameter 'show_edge_labels'.");
            }
            bShowEdgeLabels = parsed;
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(labelsBinding));

        return bindings;
    }

    auto FOntologyGraphArtistV2::SetProjection(const OntologyV2::FOntologyGraphProjection &projection) -> void {
        Projection = projection;
        bShowEdgeLabels = projection.Filters.bShowEdgeLabels;
        RebuildIndexes();
        NormalizeSelection();
    }

    auto FOntologyGraphArtistV2::GetProjection() const -> const OntologyV2::FOntologyGraphProjection & {
        return Projection;
    }

    auto FOntologyGraphArtistV2::SetSelection(const OntologyV2::FOntologyGraphSelectionV2 &selection) -> void {
        SelectedSelection = selection;
        NormalizeSelection();
    }

    auto FOntologyGraphArtistV2::GetSelection() const -> const OntologyV2::FOntologyGraphSelectionV2 & {
        return SelectedSelection;
    }

    auto FOntologyGraphArtistV2::SetShowEdgeLabels(const bool showEdgeLabels) -> void {
        bShowEdgeLabels = showEdgeLabels;
    }

    auto FOntologyGraphArtistV2::GetShowEdgeLabels() const -> bool {
        return bShowEdgeLabels;
    }

    auto FOntologyGraphArtistV2::SetOnSelectionChanged(
        std::function<void(const OntologyV2::FOntologyGraphSelectionV2 &)> callback) -> void {
        OnSelectionChanged = std::move(callback);
    }

    auto FOntologyGraphArtistV2::EmitDrawCommands(const FPlotFrameContextV2 &frame,
                                                  FPlotDrawListV2 &drawList) const -> void {
        std::set<Str> highlightedNodeIds;
        std::set<Str> highlightedEdgeIds;
        BuildHighlightSets(Projection, SelectedSelection, highlightedNodeIds, highlightedEdgeIds);

        const auto graphTextScale = ResolveOntologyTextScale(frame, Projection);
        const auto pixelSize = PixelSizeInSpace(frame.PlotRegion, frame.Viewport);
        const auto paddingX = ResolveNodePaddingX(frame, graphTextScale);
        const auto paddingY = ResolveNodePaddingY(frame, graphTextScale);
        const auto lineAdvance = ResolveNodeLineAdvance(frame, graphTextScale);
        const auto edgeLabelOffset = PixelVectorToPlotDelta({0.65 * paddingX, 0.45 * lineAdvance}, pixelSize);
        const auto textPad = PixelVectorToPlotDelta({paddingX, paddingY}, pixelSize);
        const auto baselinePad = PixelVectorToPlotDelta({0.0, 0.18 * lineAdvance}, pixelSize);
        const auto badgeTextPad = PixelVectorToPlotDelta({0.6 * paddingX, 0.3 * lineAdvance}, pixelSize);

        for (const auto &edge : Projection.Edges) {
            const auto *source = FindNode(edge.FromNodeId);
            const auto *target = FindNode(edge.ToNodeId);
            if (source == nullptr || target == nullptr) continue;

            const auto sourceRect = MeasureNodeRect(frame, *source, graphTextScale);
            const auto targetRect = MeasureNodeRect(frame, *target, graphTextScale);
            const auto route = BuildEdgeRoute(sourceRect, targetRect, pixelSize);

            auto edgeColor = edge.ActivationStatus != OntologyV2::EOntologyActivationStatusV2::None
                ? StatusAccentColor(edge.ActivationStatus)
                : TypeColor(edge.Type);
            if (!edge.bStudyOwned) {
                edgeColor = MixColor(edgeColor, FColor::FromHex("#243245"), 0.48).WithAlpha(0.88f);
            }

            const bool bSelected = SelectedSelection.Kind == OntologyV2::EOntologyElementKindV2::Edge &&
                SelectedSelection.ElementId == edge.EdgeId;
            const bool bHovered = HoveredSelection.Kind == OntologyV2::EOntologyElementKindV2::Edge &&
                HoveredSelection.ElementId == edge.EdgeId;
            const bool bHighlighted = highlightedEdgeIds.contains(edge.EdgeId);

            auto style = PlotStyle(
                edgeColor.WithAlpha(bSelected || bHovered ? 1.0f : (edge.bStudyOwned ? 0.92f : 0.72f)),
                LineStrip,
                false,
                Nil,
                static_cast<float>(bSelected ? 2.8f : (bHovered || bHighlighted ? 2.1f : (edge.bStudyOwned ? 1.6f : 1.2f))));

            drawList.AddPolyline(FPolylineCommandV2{
                .Points = route.Points,
                .Style = style,
                .bClosed = false,
                .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
            });

            if (route.ArrowPoints.has_value()) {
                drawList.AddPolyline(FPolylineCommandV2{
                    .Points = {
                        (*route.ArrowPoints)[0],
                        (*route.ArrowPoints)[1],
                        (*route.ArrowPoints)[2]
                    },
                    .Style = style,
                    .bClosed = false,
                    .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
                });
            }

            if (bShowEdgeLabels || bSelected || bHovered) {
                drawList.AddText(FTextCommandV2{
                    .Text = CompactLabel(EdgeBadgeLabel(edge.Type), 18),
                    .Location = route.LabelPoint.WithTranslation(edgeLabelOffset.x, edgeLabelOffset.y),
                    .Color = edgeColor.WithAlpha(bSelected || bHovered ? 0.98f : 0.82f),
                    .FontScale = graphTextScale,
                    .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
                });
            }
        }

        for (const auto &node : Projection.Nodes) {
            const auto rect = MeasureNodeRect(frame, node, graphTextScale);

            auto fillColor = LayerColor(node.Layer);
            if (node.OwnershipScope == OntologyV2::EOntologyOwnershipScopeV2::Global) {
                fillColor = MixColor(fillColor, FColor::FromHex("#141C29"), 0.56).WithAlpha(0.84f);
            } else {
                fillColor = MixColor(fillColor, FColor::FromHex("#28361A"), 0.18).WithAlpha(0.96f);
            }
            if (node.bAbstract) {
                fillColor = MixColor(fillColor, FColor::FromHex("#4B5260"), 0.28);
            }

            auto borderColor = node.ActivationStatus != OntologyV2::EOntologyActivationStatusV2::None
                ? StatusAccentColor(node.ActivationStatus)
                : LayerColor(node.Layer);

            const bool bSelected = SelectedSelection.Kind == OntologyV2::EOntologyElementKindV2::Node &&
                SelectedSelection.ElementId == node.NodeId;
            const bool bHovered = HoveredSelection.Kind == OntologyV2::EOntologyElementKindV2::Node &&
                HoveredSelection.ElementId == node.NodeId;
            const bool bHighlighted = highlightedNodeIds.contains(node.NodeId);

            if (bSelected) {
                borderColor = MixColor(borderColor, White, 0.20f);
                fillColor = MixColor(fillColor, borderColor, 0.12f);
            } else if (bHovered || bHighlighted) {
                borderColor = MixColor(borderColor, White, 0.10f);
            }

            drawList.AddRectangle(FRectangleCommandV2{
                .Rectangle = rect,
                .Color = fillColor,
                .bFilled = true,
                .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
            });

            drawList.AddPolyline(FPolylineCommandV2{
                .Points = BuildRectOutlinePoints(rect),
                .Style = PlotStyle(
                    borderColor.WithAlpha(bSelected ? 1.0f : 0.94f),
                    LineStrip,
                    false,
                    Nil,
                    static_cast<float>(bSelected ? 2.4f : (bHovered || bHighlighted ? 1.9f : 1.3f))),
                .bClosed = true,
                .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
            });

            if (node.bStudyRoot) {
                const auto studyBadgeWidth = PixelVectorToPlotDelta({ResolveStudyBadgeWidth(frame, graphTextScale), 0.0}, pixelSize).x;
                const auto studyBadgeHeight = PixelVectorToPlotDelta({0.0, ResolveStudyBadgeHeight(frame, graphTextScale)}, pixelSize).y;
                const RectR studyBadgeRect{
                    rect.xMin,
                    rect.xMin + studyBadgeWidth,
                    rect.yMax - studyBadgeHeight,
                    rect.yMax
                };
                drawList.AddRectangle(FRectangleCommandV2{
                    .Rectangle = studyBadgeRect,
                    .Color = FColor::FromHex("#D8D06A").WithAlpha(0.92f),
                    .bFilled = true,
                    .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
                });
                drawList.AddText(FTextCommandV2{
                    .Text = "study",
                    .Location = {studyBadgeRect.xMin + badgeTextPad.x, studyBadgeRect.yMin + badgeTextPad.y},
                    .Color = Black.WithAlpha(0.96f),
                    .FontScale = graphTextScale,
                    .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
                });
            } else {
                const auto badgeLabel = OwnershipBadgeLabel(node);
                const auto badgeWidth = PixelVectorToPlotDelta({
                    ResolveOwnershipBadgeWidth(frame, badgeLabel, graphTextScale),
                    0.0}, pixelSize).x;
                const auto badgeHeight = PixelVectorToPlotDelta({
                    0.0,
                    ResolveOwnershipBadgeHeight(frame, graphTextScale)}, pixelSize).y;
                const auto badgeFill = node.OwnershipScope == OntologyV2::EOntologyOwnershipScopeV2::Global
                    ? FColor::FromHex("#495969").WithAlpha(0.84f)
                    : FColor::FromHex("#D7C96E").WithAlpha(0.92f);
                const auto badgeTextColor = node.OwnershipScope == OntologyV2::EOntologyOwnershipScopeV2::Global
                    ? White.WithAlpha(0.94f)
                    : Black.WithAlpha(0.96f);
                const RectR ownershipBadgeRect{
                    rect.xMin,
                    rect.xMin + badgeWidth,
                    rect.yMax - badgeHeight,
                    rect.yMax
                };
                drawList.AddRectangle(FRectangleCommandV2{
                    .Rectangle = ownershipBadgeRect,
                    .Color = badgeFill,
                    .bFilled = true,
                    .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
                });
                drawList.AddText(FTextCommandV2{
                    .Text = badgeLabel,
                    .Location = {ownershipBadgeRect.xMin + badgeTextPad.x, ownershipBadgeRect.yMin + badgeTextPad.y},
                    .Color = badgeTextColor,
                    .FontScale = graphTextScale,
                    .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
                });
            }

            if (node.ActivationStatus != OntologyV2::EOntologyActivationStatusV2::None) {
                const auto badgeLabel = CompactLabel(OntologyV2::ToString(node.ActivationStatus), 14);
                const auto badgeWidth = PixelVectorToPlotDelta({
                    ResolveStatusBadgeWidth(frame, badgeLabel, graphTextScale),
                    0.0}, pixelSize).x;
                const auto badgeHeight = PixelVectorToPlotDelta({
                    0.0,
                    ResolveStatusBadgeHeight(frame, graphTextScale)}, pixelSize).y;
                const RectR badgeRect{
                    rect.xMax - badgeWidth,
                    rect.xMax,
                    rect.yMax - badgeHeight,
                    rect.yMax
                };
                const auto badgeColor = StatusAccentColor(node.ActivationStatus);
                drawList.AddRectangle(FRectangleCommandV2{
                    .Rectangle = badgeRect,
                    .Color = badgeColor.WithAlpha(0.94f),
                    .bFilled = true,
                    .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
                });
                drawList.AddText(FTextCommandV2{
                    .Text = badgeLabel,
                    .Location = {badgeRect.xMin + badgeTextPad.x, badgeRect.yMin + badgeTextPad.y},
                    .Color = Black.WithAlpha(0.98f),
                    .FontScale = graphTextScale,
                    .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
                });
            }

            const auto leadingBadgeReserve = node.bStudyRoot
                ? PixelVectorToPlotDelta({ResolveStudyBadgeWidth(frame, graphTextScale) + (0.55 * paddingX), 0.0}, pixelSize).x
                : PixelVectorToPlotDelta({
                    ResolveOwnershipBadgeWidth(frame, OwnershipBadgeLabel(node), graphTextScale) + (0.55 * paddingX),
                    0.0}, pixelSize).x;
            const auto footerBaselineY = rect.yMin + textPad.y + baselinePad.y;
            const auto titleBaselineY = rect.yMax - textPad.y - baselinePad.y;
            const auto footerRightText = CompactLabel(node.Kind, 18);
            const auto footerRightWidth = PixelVectorToPlotDelta({
                ApproximateNodeTextWidth(frame, footerRightText, 18, graphTextScale) + (0.15 * paddingX),
                0.0}, pixelSize).x;

            drawList.AddText(FTextCommandV2{
                .Text = CompactLabel(node.Title, 38),
                .Location = {rect.xMin + textPad.x + leadingBadgeReserve, titleBaselineY},
                .Color = White.WithAlpha(node.OwnershipScope == OntologyV2::EOntologyOwnershipScopeV2::StudyLocal ? 1.0f : 0.92f),
                .FontScale = graphTextScale,
                .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
            });

            drawList.AddText(FTextCommandV2{
                .Text = CompactLabel(node.SecondarySummary.empty() ? node.CompactId : node.SecondarySummary, 36),
                .Location = {rect.xMin + textPad.x, footerBaselineY},
                .Color = LightGrey.WithAlpha(0.88f),
                .FontScale = graphTextScale,
                .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
            });

            drawList.AddText(FTextCommandV2{
                .Text = footerRightText,
                .Location = {rect.xMax - textPad.x - footerRightWidth, footerBaselineY},
                .Color = borderColor.WithAlpha(0.92f),
                .FontScale = graphTextScale,
                .CoordinateSpace = EPlotCoordinateSpaceV2::Plot
            });
        }

        DevFloat hudBottom = frame.HudLayout.BottomLeft.y;
        if (SelectedSelection.IsValid()) {
            Vector<Str> lines;
            FColor accent = FColor::FromHex("#F2C66D");
            if (SelectedSelection.Kind == OntologyV2::EOntologyElementKindV2::Node) {
                if (const auto *node = FindNode(SelectedSelection.ElementId); node != nullptr) {
                    lines.push_back(node->NodeId);
                    lines.push_back(node->Summary);
                    if (!node->SecondarySummary.empty()) lines.push_back(node->SecondarySummary);
                    lines.push_back(Str("Ownership: ") + OntologyV2::ToString(node->OwnershipScope));
                    accent = node->ActivationStatus != OntologyV2::EOntologyActivationStatusV2::None
                        ? StatusAccentColor(node->ActivationStatus)
                        : LayerColor(node->Layer);
                    hudBottom = AddHudCardFromBottomLeft(
                        drawList,
                        frame,
                        frame.HudLayout.BottomLeft.x,
                        hudBottom,
                        "Selected Node",
                        lines,
                        accent);
                }
            } else if (SelectedSelection.Kind == OntologyV2::EOntologyElementKindV2::Edge) {
                if (const auto *edge = FindEdge(SelectedSelection.ElementId); edge != nullptr) {
                    lines.push_back(edge->EdgeId);
                    lines.push_back(edge->FromNodeId + " -> " + edge->ToNodeId);
                    lines.push_back(edge->Summary);
                    accent = edge->ActivationStatus != OntologyV2::EOntologyActivationStatusV2::None
                        ? StatusAccentColor(edge->ActivationStatus)
                        : TypeColor(edge->Type);
                    hudBottom = AddHudCardFromBottomLeft(
                        drawList,
                        frame,
                        frame.HudLayout.BottomLeft.x,
                        hudBottom,
                        "Selected Edge",
                        lines,
                        accent);
                }
            }
        }

        if (HoveredSelection.IsValid() && !SelectionEquals(HoveredSelection, SelectedSelection)) {
            Vector<Str> lines;
            FColor accent = FColor::FromHex("#8CD8F8");
            if (HoveredSelection.Kind == OntologyV2::EOntologyElementKindV2::Node) {
                if (const auto *node = FindNode(HoveredSelection.ElementId); node != nullptr) {
                    lines.push_back(node->NodeId);
                    lines.push_back(node->Summary);
                    if (!node->SecondarySummary.empty()) lines.push_back(node->SecondarySummary);
                    accent = node->ActivationStatus != OntologyV2::EOntologyActivationStatusV2::None
                        ? StatusAccentColor(node->ActivationStatus)
                        : LayerColor(node->Layer);
                    hudBottom = AddHudCardFromBottomLeft(
                        drawList,
                        frame,
                        frame.HudLayout.BottomLeft.x,
                        hudBottom,
                        "Hover Node",
                        lines,
                        accent);
                }
            } else if (HoveredSelection.Kind == OntologyV2::EOntologyElementKindV2::Edge) {
                if (const auto *edge = FindEdge(HoveredSelection.ElementId); edge != nullptr) {
                    lines.push_back(edge->EdgeId);
                    lines.push_back(edge->FromNodeId + " -> " + edge->ToNodeId);
                    lines.push_back(edge->Summary);
                    accent = edge->ActivationStatus != OntologyV2::EOntologyActivationStatusV2::None
                        ? StatusAccentColor(edge->ActivationStatus)
                        : TypeColor(edge->Type);
                    hudBottom = AddHudCardFromBottomLeft(
                        drawList,
                        frame,
                        frame.HudLayout.BottomLeft.x,
                        hudBottom,
                        "Hover Edge",
                        lines,
                        accent);
                }
            }
        }

        DevFloat hudTop = frame.HudLayout.TopRight.y;
        const auto blockedCount = static_cast<std::size_t>(std::count_if(
            Projection.Nodes.begin(),
            Projection.Nodes.end(),
            [](const auto &node) {
                return node.ActivationStatus == OntologyV2::EOntologyActivationStatusV2::Blocked;
            }));
        const auto studyNodeCount = static_cast<std::size_t>(std::count_if(
            Projection.Nodes.begin(),
            Projection.Nodes.end(),
            [](const auto &node) {
                return node.OwnershipScope == OntologyV2::EOntologyOwnershipScopeV2::StudyLocal;
            }));
        Vector<Str> statusLines;
        statusLines.push_back("Study: " + (Projection.SelectedStudyId.empty() ? Str("none") : Projection.SelectedStudyId));
        statusLines.push_back("Nodes: " + ToStr(Projection.Nodes.size()) + " | Edges: " + ToStr(Projection.Edges.size()));
        statusLines.push_back("Study-local: " + ToStr(studyNodeCount) + " | Blocked: " + ToStr(blockedCount));
        statusLines.push_back("Scope: " + Str(OntologyV2::ToString(Projection.Filters.ScopeMode)));
        statusLines.push_back("Diagnostics: " + ToStr(Projection.Diagnostics.ErrorCount()) + " errors, " +
            ToStr(Projection.Diagnostics.WarningCount()) + " warnings");
        hudTop = AddHudCardFromTopRight(
            drawList,
            frame,
            frame.HudLayout.TopRight.x,
            hudTop,
            "Ontology Graph",
            statusLines,
            FColor::FromHex("#7DA8D8"));
    }

    auto FOntologyGraphArtistV2::ComputePlotBounds() const -> std::optional<RectR> {
        return ComputeProjectionBounds(Projection);
    }

    auto FOntologyGraphArtistV2::HitTest(const FPlotFrameContextV2 &frame,
                                         const FPoint2D &plotPosition,
                                         const FPoint2D &viewportPosition) const
        -> std::optional<FPlotHitTargetV2> {
        (void) viewportPosition;

        const auto graphTextScale = ResolveOntologyTextScale(frame, Projection);

        for (const auto &node : Projection.Nodes) {
            const auto rect = MeasureNodeRect(frame, node, graphTextScale);
            if (plotPosition.x < rect.xMin || plotPosition.x > rect.xMax ||
                plotPosition.y < rect.yMin || plotPosition.y > rect.yMax) {
                continue;
            }

            return FPlotHitTargetV2{
                .TargetId = node.NodeId,
                .DisplayLabel = node.Title,
                .Detail = node.Summary
            };
        }

        const auto pixelSize = PixelSizeInSpace(frame.PlotRegion, frame.Viewport);
        const auto toleranceSquared = CEdgeHitTolerancePixels * CEdgeHitTolerancePixels;
        const OntologyV2::FOntologyProjectedEdgeV2 *bestEdge = nullptr;
        DevFloat bestDistance = std::numeric_limits<DevFloat>::max();
        for (const auto &edge : Projection.Edges) {
            const auto *source = FindNode(edge.FromNodeId);
            const auto *target = FindNode(edge.ToNodeId);
            if (source == nullptr || target == nullptr) continue;

            const auto route = BuildEdgeRoute(
                MeasureNodeRect(frame, *source, graphTextScale),
                MeasureNodeRect(frame, *target, graphTextScale),
                pixelSize);
            for (std::size_t index = 1; index < route.Points.size(); ++index) {
                const auto distance = DistanceToSegmentSquaredPixels(
                    plotPosition,
                    route.Points[index - 1],
                    route.Points[index],
                    pixelSize);
                if (distance > toleranceSquared || distance >= bestDistance) continue;
                bestDistance = distance;
                bestEdge = &edge;
            }
        }

        if (bestEdge == nullptr) return std::nullopt;
        return FPlotHitTargetV2{
            .TargetId = bestEdge->EdgeId,
            .DisplayLabel = bestEdge->Type,
            .Detail = bestEdge->Summary
        };
    }

    auto FOntologyGraphArtistV2::HandlePointerEvent(const FPlotFrameContextV2 &frame,
                                                    const FPlotPointerEventV2 &event) -> bool {
        const auto hit = HitTest(frame, event.PlotPosition, event.ViewportPosition);
        OntologyV2::FOntologyGraphSelectionV2 hitSelection;
        if (hit.has_value()) {
            if (NodeIndexById.contains(hit->TargetId)) {
                hitSelection = {OntologyV2::EOntologyElementKindV2::Node, hit->TargetId};
            } else if (EdgeIndexById.contains(hit->TargetId)) {
                hitSelection = {OntologyV2::EOntologyElementKindV2::Edge, hit->TargetId};
            }
        }

        switch (event.Kind) {
            case EPlotPointerEventKindV2::Move:
                HoveredSelection = hitSelection;
                return hitSelection.IsValid();

            case EPlotPointerEventKindV2::Leave:
                HoveredSelection = {};
                PressedSelection = {};
                return false;

            case EPlotPointerEventKindV2::Wheel:
                return false;

            case EPlotPointerEventKindV2::Button:
                if (event.Button != MouseButton_LEFT) return false;
                if (event.ButtonState == Press) {
                    PressedSelection = hitSelection;
                    HoveredSelection = hitSelection;
                    return hitSelection.IsValid();
                }

                if (event.ButtonState == Release) {
                    HoveredSelection = hitSelection;
                    const bool bSameSelection = hitSelection.IsValid() && SelectionEquals(hitSelection, PressedSelection);
                    PressedSelection = {};

                    if (bSameSelection) {
                        SelectedSelection = hitSelection;
                        if (OnSelectionChanged) OnSelectionChanged(SelectedSelection);
                        return true;
                    }

                    if (!hitSelection.IsValid() && SelectedSelection.IsValid()) {
                        SelectedSelection = {};
                        if (OnSelectionChanged) OnSelectionChanged(SelectedSelection);
                    }
                }
                return false;
        }

        return false;
    }

    auto FOntologyGraphArtistV2::HandleKeyboardEvent(const FPlotFrameContextV2 &frame,
                                                     const FPlotKeyboardEventV2 &event) -> bool {
        (void) frame;
        if (event.State == Release) return false;

        switch (event.Key) {
            case Key_L:
            case Key_l:
                bShowEdgeLabels = !bShowEdgeLabels;
                return true;

            default:
                break;
        }

        return false;
    }

    auto FOntologyGraphArtistV2::GetArtistTypeId() const -> Str {
        return "ontology_graph";
    }

} // namespace Slab::Graphics::Plot2D::V2
