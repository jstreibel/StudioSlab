#ifndef STUDIOSLAB_MODEL_SEMANTIC_GRAPH_ARTIST_V2_H
#define STUDIOSLAB_MODEL_SEMANTIC_GRAPH_ARTIST_V2_H

#include "Core/Model/V2/ModelAuthoringV2.h"
#include "Graphics/Plot2D/V2/PlotArtistV2.h"

#include <functional>
#include <map>
#include <optional>

namespace Slab::Graphics::Plot2D::V2 {

    class FModelSemanticGraphArtistV2 final : public FPlotArtistV2 {
    public:
        enum class EEdgeKind : unsigned char {
            Dependency,
            AmbientDependency,
            SourceLink,
            TargetLink,
            Assumption,
            Override
        };

        struct FGraphNodeV2 {
            Slab::Core::Model::V2::FSemanticObjectRefV2 Ref;
            Str NodeId;
            Str Label;
            Str FullLabel;
            Str Subtitle;
            Str KindLabel;
            Str CanonicalNotation;
            Str Description;
            Slab::Core::Model::V2::ESemanticObjectKindV2 Kind =
                Slab::Core::Model::V2::ESemanticObjectKindV2::Definition;
            bool bReadonly = false;
            bool bAmbient = false;
            bool bConflict = false;
            bool bLocalOverride = false;
            int HopDistance = 0;
            FPoint2D Position{};
            DevFloat BasePointSize = 11.0;
        };

        struct FGraphEdgeV2 {
            Str EdgeId;
            Str SourceNodeId;
            Str TargetNodeId;
            EEdgeKind Kind = EEdgeKind::Dependency;
            Str Label;
            Str Detail;
            bool bAmbient = false;
            bool bConflict = false;
            bool bReadonly = false;
            bool bOverride = false;
        };

    private:
        Vector<FGraphNodeV2> Nodes;
        Vector<FGraphEdgeV2> Edges;
        std::map<Str, std::size_t> NodeIndexById;
        std::optional<Slab::Core::Model::V2::FModelSemanticOverviewV2> LastOverview;
        Slab::Core::Model::V2::FSemanticObjectRefV2 SelectedObject;
        Str HoveredNodeId;
        Str HoveredEdgeId;
        Str PressedNodeId;
        int NeighborhoodHops = 2;
        bool bShowLabels = true;
        std::function<void(const Slab::Core::Model::V2::FSemanticObjectRefV2 &)> OnSemanticObjectActivated;

        auto RebuildLayout() -> void;
        [[nodiscard]] auto FindNodeById(const Str &nodeId) const -> const FGraphNodeV2 *;
        [[nodiscard]] auto FindEdgeById(const Str &edgeId) const -> const FGraphEdgeV2 *;
        [[nodiscard]] auto FindEdgeHitId(const FPlotFrameContextV2 &frame, const FPoint2D &plotPosition) const -> Str;

    protected:
        [[nodiscard]] auto BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> override;

    public:
        FModelSemanticGraphArtistV2();

        auto SetSemanticOverview(const Slab::Core::Model::V2::FModelSemanticOverviewV2 &overview,
                                 const Slab::Core::Model::V2::FSemanticObjectRefV2 &selectedObject) -> void;

        auto SetNeighborhoodHops(int hops) -> void;
        [[nodiscard]] auto GetNeighborhoodHops() const -> int;

        auto SetShowLabels(bool showLabels) -> void;
        [[nodiscard]] auto GetShowLabels() const -> bool;

        auto SetOnSemanticObjectActivated(
            std::function<void(const Slab::Core::Model::V2::FSemanticObjectRefV2 &)> callback) -> void;

        [[nodiscard]] auto GetNodes() const -> const Vector<FGraphNodeV2> &;
        [[nodiscard]] auto GetEdges() const -> const Vector<FGraphEdgeV2> &;

        auto EmitDrawCommands(const FPlotFrameContextV2 &frame,
                              FPlotDrawListV2 &drawList) const -> void override;

        [[nodiscard]] auto ComputePlotBounds() const -> std::optional<RectR> override;
        [[nodiscard]] auto HitTest(const FPlotFrameContextV2 &frame,
                                   const FPoint2D &plotPosition,
                                   const FPoint2D &viewportPosition) const
            -> std::optional<FPlotHitTargetV2> override;
        auto HandlePointerEvent(const FPlotFrameContextV2 &frame,
                                const FPlotPointerEventV2 &event) -> bool override;
        auto HandleKeyboardEvent(const FPlotFrameContextV2 &frame,
                                 const FPlotKeyboardEventV2 &event) -> bool override;
        [[nodiscard]] auto GetArtistTypeId() const -> Str override;
    };

    DefinePointers(FModelSemanticGraphArtistV2)

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_MODEL_SEMANTIC_GRAPH_ARTIST_V2_H
