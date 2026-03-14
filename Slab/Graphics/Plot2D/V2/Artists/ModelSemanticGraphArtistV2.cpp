#include "ModelSemanticGraphArtistV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"
#include "Graphics/Utils.h"

#include <algorithm>
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
        constexpr DevFloat CHudPaddingX = 12.0;
        constexpr DevFloat CHudPaddingY = 10.0;
        constexpr DevFloat CHudLineHeight = 15.0;
        constexpr DevFloat CHudSectionGap = 10.0;
        constexpr DevFloat CHudCardGap = 10.0;
        constexpr DevFloat CHudApproxCharWidth = 7.0;

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

        [[nodiscard]] auto ApproximateHudTextWidth(const Str &text) -> DevFloat {
            return static_cast<DevFloat>(text.size()) * CHudApproxCharWidth;
        }

        [[nodiscard]] auto DisplayLabelForObject(const ModelV2::FSemanticObjectOverviewV2 &object) -> Str {
            if (!object.DisplayLabel.empty()) return CompactLabel(object.DisplayLabel);
            if (!object.CanonicalNotation.empty()) return CompactLabel(object.CanonicalNotation);
            return CompactLabel(object.Ref.ObjectId);
        }

        [[nodiscard]] auto FullDisplayLabelForObject(const ModelV2::FSemanticObjectOverviewV2 &object) -> Str {
            if (!object.DisplayLabel.empty()) return object.DisplayLabel;
            if (!object.CanonicalNotation.empty()) return object.CanonicalNotation;
            return object.Ref.ObjectId;
        }

        [[nodiscard]] auto SubtitleForObject(const ModelV2::FSemanticObjectOverviewV2 &object) -> Str {
            if (!object.StatusLabel.empty()) return object.StatusLabel;
            if (!object.CategoryLabel.empty()) return object.CategoryLabel;
            return object.KindLabel;
        }

        [[nodiscard]] auto BasePointSizeForObject(const ModelV2::FSemanticObjectOverviewV2 &object) -> DevFloat {
            switch (object.Ref.Kind) {
                case ModelV2::ESemanticObjectKindV2::Definition: return 12.0;
                case ModelV2::ESemanticObjectKindV2::Relation: return 13.5;
                case ModelV2::ESemanticObjectKindV2::Assumption: return 11.5;
                case ModelV2::ESemanticObjectKindV2::VocabularyEntry: return 10.5;
            }

            return 11.0;
        }

        [[nodiscard]] auto BaseColorForNode(const FModelSemanticGraphArtistV2::FGraphNodeV2 &node) -> FColor {
            switch (node.Kind) {
                case ModelV2::ESemanticObjectKindV2::Definition:
                    return FColor::FromHex("#E2A33B");
                case ModelV2::ESemanticObjectKindV2::Relation:
                    return FColor::FromHex("#4FA7A2");
                case ModelV2::ESemanticObjectKindV2::Assumption:
                    return FColor::FromHex("#D56A4E");
                case ModelV2::ESemanticObjectKindV2::VocabularyEntry:
                    return FColor::FromHex("#5C8BCB");
            }

            return LightGrey;
        }

        [[nodiscard]] auto LabelColorForNode(const FModelSemanticGraphArtistV2::FGraphNodeV2 &node) -> FColor {
            auto color = node.bAmbient ? LightGrey : White;
            if (node.bConflict) color = FColor::FromHex("#F08473");
            if (node.bLocalOverride) color = FColor::FromHex("#F5D37A");
            return color;
        }

        [[nodiscard]] auto HaloColorForNode(const FModelSemanticGraphArtistV2::FGraphNodeV2 &node,
                                            const bool bSelected,
                                            const bool bHovered) -> FColor {
            if (bSelected) return FColor::FromHex("#FFF0A8").WithAlpha(0.34f);
            if (bHovered) return FColor::FromHex("#A8E6FF").WithAlpha(0.26f);
            if (node.bConflict) return FColor::FromHex("#F08473").WithAlpha(0.24f);
            if (node.bAmbient) return FColor::FromHex("#9DB7DA").WithAlpha(0.16f);
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
            const auto color = EdgeColorForKind(edge).WithAlpha(edge.bAmbient ? 0.72f : 0.84f);
            auto style = PlotStyle(color, LineStrip, false, Nil, edge.bAmbient ? 1.0f : 1.35f);
            return style;
        }

        [[nodiscard]] auto EdgeKindLabel(const FModelSemanticGraphArtistV2::EEdgeKind kind) -> Str {
            switch (kind) {
                case FModelSemanticGraphArtistV2::EEdgeKind::Dependency: return "Depends On";
                case FModelSemanticGraphArtistV2::EEdgeKind::AmbientDependency: return "Ambient Dependency";
                case FModelSemanticGraphArtistV2::EEdgeKind::SourceLink: return "Source Link";
                case FModelSemanticGraphArtistV2::EEdgeKind::TargetLink: return "Target Link";
                case FModelSemanticGraphArtistV2::EEdgeKind::Assumption: return "Assumption";
                case FModelSemanticGraphArtistV2::EEdgeKind::Override: return "Override";
            }

            return "Link";
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

            const auto t = std::clamp(((apx * abx) + (apy * aby)) / abLengthSquared, static_cast<DevFloat>(0.0), static_cast<DevFloat>(1.0));
            const auto closestX = scaledAx + (t * abx);
            const auto closestY = scaledAy + (t * aby);
            const auto dx = scaledPx - closestX;
            const auto dy = scaledPy - closestY;
            return (dx * dx) + (dy * dy);
        }

        auto AddHudCard(FPlotDrawListV2 &drawList,
                        const FPlotFrameContextV2 &frame,
                        const DevFloat xMin,
                        DevFloat yTop,
                        const Str &title,
                        const Vector<Str> &lines,
                        const FColor &accentColor) -> DevFloat {
            (void) frame;
            if (title.empty() && lines.empty()) return yTop;

            DevFloat width = ApproximateHudTextWidth(title);
            for (const auto &line : lines) {
                width = std::max(width, ApproximateHudTextWidth(line));
            }

            width += (2.0 * CHudPaddingX);
            const auto lineCount = static_cast<DevFloat>(std::max<std::size_t>(1, lines.size()));
            const auto height =
                (2.0 * CHudPaddingY) + CHudLineHeight + ((lineCount - 1.0) * CHudLineHeight) + CHudSectionGap;

            const RectR backgroundRect{
                xMin,
                xMin + width,
                yTop - height,
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

            const auto textX = xMin + CHudPaddingX;
            auto textY = yTop - CHudPaddingY - CHudLineHeight;
            drawList.AddText(FTextCommandV2{
                .Text = title,
                .Location = {textX, textY},
                .Color = accentColor,
                .CoordinateSpace = EPlotCoordinateSpaceV2::Screen
            });

            for (const auto &line : lines) {
                textY -= CHudLineHeight;
                drawList.AddText(FTextCommandV2{
                    .Text = CompactLabel(line, 84),
                    .Location = {textX, textY},
                    .Color = White.WithAlpha(0.95f),
                    .CoordinateSpace = EPlotCoordinateSpaceV2::Screen
                });
            }

            return backgroundRect.yMin - CHudCardGap;
        }

        [[nodiscard]] auto AppendTraversalTargets(const ModelV2::FSemanticObjectOverviewV2 &object)
            -> Vector<ModelV2::FSemanticObjectRefV2> {
            Vector<ModelV2::FSemanticObjectRefV2> refs;
            refs.reserve(
                object.DependsOn.size() +
                object.AmbientDependencies.size() +
                object.UsedBy.size() +
                object.RelatedAssumptions.size() +
                object.SourceLinks.size() +
                object.TargetLinks.size() +
                object.LocalOverrides.size());

            const auto appendLinks = [&refs](const auto &links) {
                for (const auto &link : links) refs.push_back(link.Target);
            };

            appendLinks(object.DependsOn);
            appendLinks(object.AmbientDependencies);
            appendLinks(object.UsedBy);
            appendLinks(object.RelatedAssumptions);
            appendLinks(object.SourceLinks);
            appendLinks(object.TargetLinks);
            appendLinks(object.LocalOverrides);
            return refs;
        }

    } // namespace

    FModelSemanticGraphArtistV2::FModelSemanticGraphArtistV2()
    : FPlotArtistV2("model_semantic_graph", "Model Semantic Graph") {
    }

    auto FModelSemanticGraphArtistV2::SetSemanticOverview(const ModelV2::FModelSemanticOverviewV2 &overview,
                                                          const ModelV2::FSemanticObjectRefV2 &selectedObject) -> void {
        LastOverview = overview;
        Nodes.clear();
        Edges.clear();
        NodeIndexById.clear();
        HoveredNodeId.clear();
        HoveredEdgeId.clear();
        PressedNodeId.clear();

        if (overview.ObjectsByKey.empty()) {
            SelectedObject = {};
            return;
        }

        auto centerRef = selectedObject;
        if (!centerRef.IsValid() || overview.FindObject(centerRef) == nullptr) {
            centerRef = overview.ObjectsByKey.begin()->second.Ref;
        }
        SelectedObject = centerRef;

        std::map<Str, int> hopByKey;
        std::queue<ModelV2::FSemanticObjectRefV2> pending;
        const auto centerKey = ModelV2::MakeSemanticObjectKeyV2(centerRef);
        hopByKey[centerKey] = 0;
        pending.push(centerRef);

        while (!pending.empty()) {
            const auto currentRef = pending.front();
            pending.pop();

            const auto currentKey = ModelV2::MakeSemanticObjectKeyV2(currentRef);
            const auto hopIt = hopByKey.find(currentKey);
            if (hopIt == hopByKey.end()) continue;
            if (hopIt->second >= NeighborhoodHops) continue;

            const auto *object = overview.FindObject(currentRef);
            if (object == nullptr) continue;

            for (const auto &targetRef : AppendTraversalTargets(*object)) {
                const auto *targetObject = overview.FindObject(targetRef);
                if (targetObject == nullptr) continue;

                const auto targetKey = ModelV2::MakeSemanticObjectKeyV2(targetRef);
                if (hopByKey.contains(targetKey)) continue;

                hopByKey[targetKey] = hopIt->second + 1;
                pending.push(targetRef);
            }
        }

        Vector<Str> orderedKeys;
        orderedKeys.reserve(hopByKey.size());
        for (const auto &[key, hop] : hopByKey) {
            (void) hop;
            orderedKeys.push_back(key);
        }

        std::sort(orderedKeys.begin(), orderedKeys.end(), [&](const Str &lhs, const Str &rhs) {
            const auto lhsHop = hopByKey.at(lhs);
            const auto rhsHop = hopByKey.at(rhs);
            if (lhsHop != rhsHop) return lhsHop < rhsHop;

            const auto *lhsObject = &overview.ObjectsByKey.at(lhs);
            const auto *rhsObject = &overview.ObjectsByKey.at(rhs);
            const auto lhsRank = KindRank(lhsObject->Ref.Kind);
            const auto rhsRank = KindRank(rhsObject->Ref.Kind);
            if (lhsRank != rhsRank) return lhsRank < rhsRank;
            if (lhsObject->DisplayLabel != rhsObject->DisplayLabel) {
                return lhsObject->DisplayLabel < rhsObject->DisplayLabel;
            }

            return lhs < rhs;
        });

        Nodes.reserve(orderedKeys.size());
        for (const auto &key : orderedKeys) {
            const auto &object = overview.ObjectsByKey.at(key);

            FGraphNodeV2 node;
            node.Ref = object.Ref;
            node.NodeId = key;
            node.Label = DisplayLabelForObject(object);
            node.FullLabel = FullDisplayLabelForObject(object);
            node.Subtitle = CompactLabel(SubtitleForObject(object), 22);
            node.KindLabel = object.KindLabel;
            node.CanonicalNotation = object.CanonicalNotation;
            node.Description = object.Description;
            node.Kind = object.Ref.Kind;
            node.bReadonly = object.bReadonly;
            node.bAmbient = object.bAmbient;
            node.bConflict = object.bConflict;
            node.bLocalOverride = object.bLocalOverride;
            node.HopDistance = hopByKey.at(key);
            node.BasePointSize = BasePointSizeForObject(object);

            NodeIndexById[node.NodeId] = Nodes.size();
            Nodes.push_back(std::move(node));
        }

        const auto addEdges = [&](const ModelV2::FSemanticObjectOverviewV2 &object,
                                  const auto &links,
                                  const EEdgeKind kind,
                                  std::set<Str> &edgeKeys) {
            const auto sourceKey = ModelV2::MakeSemanticObjectKeyV2(object.Ref);
            if (!NodeIndexById.contains(sourceKey)) return;

            for (const auto &link : links) {
                const auto targetKey = ModelV2::MakeSemanticObjectKeyV2(link.Target);
                if (!NodeIndexById.contains(targetKey)) continue;

                const auto dedupeKey =
                    sourceKey + "::" + targetKey + "::" + ToStr(static_cast<int>(kind)) + "::" + link.Label + "::" + link.Detail;
                if (!edgeKeys.insert(dedupeKey).second) continue;

                FGraphEdgeV2 edge;
                edge.EdgeId = dedupeKey;
                edge.SourceNodeId = sourceKey;
                edge.TargetNodeId = targetKey;
                edge.Kind = kind;
                edge.Label = EdgeKindLabel(kind);
                edge.Detail = link.Detail.empty() ? link.Label : link.Detail;
                edge.bAmbient = link.bAmbient;
                edge.bConflict = link.bConflict;
                edge.bReadonly = link.bReadonly;
                edge.bOverride = link.bOverride;
                Edges.push_back(std::move(edge));
            }
        };

        std::set<Str> edgeKeys;
        for (const auto &node : Nodes) {
            const auto *object = overview.FindObject(node.Ref);
            if (object == nullptr) continue;

            addEdges(*object, object->DependsOn, EEdgeKind::Dependency, edgeKeys);
            addEdges(*object, object->AmbientDependencies, EEdgeKind::AmbientDependency, edgeKeys);
            addEdges(*object, object->SourceLinks, EEdgeKind::SourceLink, edgeKeys);
            addEdges(*object, object->TargetLinks, EEdgeKind::TargetLink, edgeKeys);
            addEdges(*object, object->RelatedAssumptions, EEdgeKind::Assumption, edgeKeys);
            addEdges(*object, object->LocalOverrides, EEdgeKind::Override, edgeKeys);
        }

        RebuildLayout();
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
            command.Points = {source->Position, target->Position};
            drawList.AddPolyline(std::move(command));

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
            DevFloat topY = static_cast<DevFloat>(frame.Viewport.GetHeight()) - 14.0;

            const auto *selectedNode = [&]() -> const FGraphNodeV2 * {
                for (const auto &node : Nodes) {
                    if (ModelV2::AreSemanticObjectRefsEqualV2(node.Ref, SelectedObject)) return &node;
                }
                return nullptr;
            }();

            if (selectedNode != nullptr) {
                Vector<Str> lines;
                lines.push_back(selectedNode->FullLabel + " [" + selectedNode->KindLabel + "]");
                if (!selectedNode->CanonicalNotation.empty()) lines.push_back(selectedNode->CanonicalNotation);
                else if (!selectedNode->Description.empty()) lines.push_back(selectedNode->Description);
                lines.push_back("Neighborhood hops: " + ToStr(NeighborhoodHops));
                topY = AddHudCard(
                    drawList,
                    frame,
                    14.0,
                    topY,
                    "Selected",
                    lines,
                    FColor::FromHex("#F2C66D"));
            }

            if (!HoveredNodeId.empty() && HoveredNodeId != (selectedNode != nullptr ? selectedNode->NodeId : Str{})) {
                if (const auto *hoveredNode = FindNodeById(HoveredNodeId); hoveredNode != nullptr) {
                    Vector<Str> lines;
                    lines.push_back(hoveredNode->FullLabel + " [" + hoveredNode->KindLabel + "]");
                    if (!hoveredNode->CanonicalNotation.empty()) lines.push_back(hoveredNode->CanonicalNotation);
                    else if (!hoveredNode->Description.empty()) lines.push_back(hoveredNode->Description);
                    if (!hoveredNode->Subtitle.empty()) lines.push_back(hoveredNode->Subtitle);
                    topY = AddHudCard(
                        drawList,
                        frame,
                        14.0,
                        topY,
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
                    if (!hoveredEdge->Detail.empty()) lines.push_back(hoveredEdge->Detail);
                    topY = AddHudCard(
                        drawList,
                        frame,
                        14.0,
                        topY,
                        "Hover Edge: " + hoveredEdge->Label,
                        lines,
                        EdgeColorForKind(*hoveredEdge));
                }
            }

            Vector<Str> statusLines;
            statusLines.push_back("Nodes: " + ToStr(Nodes.size()) + " | Edges: " + ToStr(Edges.size()));
            statusLines.push_back("Keys: [ ] adjust radius, L labels, F fit");
            (void) AddHudCard(
                drawList,
                frame,
                static_cast<DevFloat>(std::max(14, frame.Viewport.GetWidth() - 350)),
                static_cast<DevFloat>(frame.Viewport.GetHeight()) - 14.0,
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

            default:
                break;
        }

        return false;
    }

    auto FModelSemanticGraphArtistV2::GetArtistTypeId() const -> Str {
        return "model_semantic_graph";
    }

} // namespace Slab::Graphics::Plot2D::V2
