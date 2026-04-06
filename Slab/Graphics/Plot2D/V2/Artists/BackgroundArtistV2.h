#ifndef STUDIOSLAB_BACKGROUND_ARTIST_V2_H
#define STUDIOSLAB_BACKGROUND_ARTIST_V2_H

#include "Graphics/Plot2D/V2/PlotArtistV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    class FBackgroundArtistV2 final : public FPlotArtistV2 {
        FColor BackgroundColor = Black;
        bool bUseThemeColor = true;

    protected:
        [[nodiscard]] auto BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> override;

    public:
        explicit FBackgroundArtistV2(FColor backgroundColor = Black);

        auto EmitDrawCommands(const FPlotFrameContextV2 &frame,
                              FPlotDrawListV2 &drawList) const -> void override;

        [[nodiscard]] auto GetArtistTypeId() const -> Str override;

        auto SetBackgroundColor(FColor color) -> void;
        [[nodiscard]] auto GetBackgroundColor() const -> const FColor &;

        auto SetUseThemeColor(bool useThemeColor) -> void;
        [[nodiscard]] auto GetUseThemeColor() const -> bool;
    };

    DefinePointers(FBackgroundArtistV2)

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_BACKGROUND_ARTIST_V2_H
