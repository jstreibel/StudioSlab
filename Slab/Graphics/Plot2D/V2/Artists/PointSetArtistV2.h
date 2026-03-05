#ifndef STUDIOSLAB_POINTSET_ARTIST_V2_H
#define STUDIOSLAB_POINTSET_ARTIST_V2_H

#include "Graphics/Plot2D/V2/PlotArtistV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    class FPointSetArtistV2 final : public FPlotArtistV2 {
        Vector<FPoint2D> Points;
        PlotStyle Style;

    protected:
        [[nodiscard]] auto BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> override;

    public:
        explicit FPointSetArtistV2(Vector<FPoint2D> points = {},
                                   PlotStyle style = PlotStyle(White, LineStrip, false, Nil, 1.4f));

        auto EmitDrawCommands(const FPlotFrameContextV2 &frame,
                              FPlotDrawListV2 &drawList) const -> void override;

        [[nodiscard]] auto ComputePlotBounds() const -> std::optional<RectR> override;
        [[nodiscard]] auto GetArtistTypeId() const -> Str override;

        auto SetPoints(Vector<FPoint2D> points) -> void;
        auto SetStyle(PlotStyle style) -> void;

        [[nodiscard]] auto GetPoints() const -> const Vector<FPoint2D> &;
        [[nodiscard]] auto GetStyle() const -> const PlotStyle &;
    };

    DefinePointers(FPointSetArtistV2)

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_POINTSET_ARTIST_V2_H
