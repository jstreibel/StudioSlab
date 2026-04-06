#ifndef STUDIOSLAB_ONTOLOGY_GRAPH_ARTIST_V2_H
#define STUDIOSLAB_ONTOLOGY_GRAPH_ARTIST_V2_H

#include "Core/Ontology/V2/OntologyGraphV2.h"
#include "Graphics/Plot2D/V2/PlotArtistV2.h"

#include <functional>
#include <map>

namespace Slab::Graphics::Plot2D::V2 {

    enum class EOntologyGraphDisplayModeV2 : unsigned char {
        Overview,
        Focus
    };

    class FOntologyGraphArtistV2 final : public FPlotArtistV2 {
        Slab::Core::Ontology::V2::FOntologyGraphProjection Projection;
        std::map<Str, std::size_t> NodeIndexById;
        std::map<Str, std::size_t> EdgeIndexById;
        Slab::Core::Ontology::V2::FOntologyGraphSelectionV2 SelectedSelection;
        Slab::Core::Ontology::V2::FOntologyGraphSelectionV2 HoveredSelection;
        Slab::Core::Ontology::V2::FOntologyGraphSelectionV2 PressedSelection;
        EOntologyGraphDisplayModeV2 DisplayMode = EOntologyGraphDisplayModeV2::Focus;
        bool bShowEdgeLabels = true;
        std::function<void(const Slab::Core::Ontology::V2::FOntologyGraphSelectionV2 &)> OnSelectionChanged;

        auto RebuildIndexes() -> void;
        auto NormalizeSelection() -> void;

        [[nodiscard]] auto FindNode(const Str &nodeId) const
            -> const Slab::Core::Ontology::V2::FOntologyProjectedNodeV2 *;
        [[nodiscard]] auto FindEdge(const Str &edgeId) const
            -> const Slab::Core::Ontology::V2::FOntologyProjectedEdgeV2 *;

    protected:
        [[nodiscard]] auto BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> override;

    public:
        FOntologyGraphArtistV2();

        auto SetProjection(const Slab::Core::Ontology::V2::FOntologyGraphProjection &projection) -> void;
        [[nodiscard]] auto GetProjection() const -> const Slab::Core::Ontology::V2::FOntologyGraphProjection &;

        auto SetSelection(const Slab::Core::Ontology::V2::FOntologyGraphSelectionV2 &selection) -> void;
        [[nodiscard]] auto GetSelection() const -> const Slab::Core::Ontology::V2::FOntologyGraphSelectionV2 &;

        auto SetDisplayMode(EOntologyGraphDisplayModeV2 displayMode) -> void;
        [[nodiscard]] auto GetDisplayMode() const -> EOntologyGraphDisplayModeV2;

        auto SetShowEdgeLabels(bool showEdgeLabels) -> void;
        [[nodiscard]] auto GetShowEdgeLabels() const -> bool;

        auto SetOnSelectionChanged(
            std::function<void(const Slab::Core::Ontology::V2::FOntologyGraphSelectionV2 &)> callback) -> void;

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

    DefinePointers(FOntologyGraphArtistV2)

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_ONTOLOGY_GRAPH_ARTIST_V2_H
