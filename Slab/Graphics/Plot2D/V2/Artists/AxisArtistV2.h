#ifndef STUDIOSLAB_AXIS_ARTIST_V2_H
#define STUDIOSLAB_AXIS_ARTIST_V2_H

#include "Graphics/Plot2D/V2/PlotArtistV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    class FAxisArtistV2 final : public FPlotArtistV2 {
        PlotStyle AxisStyle;
        PlotStyle GridStyle;
        bool bShowGrid = true;
        bool bShowAxes = true;
        bool bShowTickMarks = true;
        bool bShowTickLabels = true;
        int MajorTickCount = 8;

    protected:
        [[nodiscard]] auto BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> override;

    public:
        FAxisArtistV2();

        auto EmitDrawCommands(const FPlotFrameContextV2 &frame,
                              FPlotDrawListV2 &drawList) const -> void override;

        [[nodiscard]] auto GetArtistTypeId() const -> Str override;

        auto SetAxisStyle(PlotStyle style) -> void;
        auto SetGridStyle(PlotStyle style) -> void;

        auto SetShowGrid(bool showGrid) -> void;
        auto SetShowAxes(bool showAxes) -> void;
        auto SetShowTickMarks(bool showTickMarks) -> void;
        auto SetShowTickLabels(bool showTickLabels) -> void;
        auto SetMajorTickCount(int tickCount) -> void;

        [[nodiscard]] auto GetShowGrid() const -> bool;
        [[nodiscard]] auto GetShowAxes() const -> bool;
        [[nodiscard]] auto GetShowTickMarks() const -> bool;
        [[nodiscard]] auto GetShowTickLabels() const -> bool;
        [[nodiscard]] auto GetMajorTickCount() const -> int;
    };

    DefinePointers(FAxisArtistV2)

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_AXIS_ARTIST_V2_H
