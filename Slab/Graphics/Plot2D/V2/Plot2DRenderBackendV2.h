#ifndef STUDIOSLAB_PLOT2D_RENDER_BACKEND_V2_H
#define STUDIOSLAB_PLOT2D_RENDER_BACKEND_V2_H

#include "Plot2DDrawListV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    struct FPlotTextMetricsV2 {
        DevFloat FontHeightPixels = 17.0;
        DevFloat LineAdvancePixels = 17.0;
        DevFloat ApproxCharacterAdvancePixels = 10.0;
    };

    struct FPlotHudLayoutV2 {
        RectR SafeRect{};
        FPoint2D TopLeft{};
        FPoint2D TopRight{};
        FPoint2D BottomLeft{};
        FPoint2D BottomRight{};
    };

    struct FPlotFrameContextV2 {
        RectR PlotRegion{};
        RectI Viewport{};
        Str WindowId;
        Str Title;
        FPlotTextMetricsV2 TextMetrics{};
        FPlotHudLayoutV2 HudLayout{};
    };

    class IPlotRenderBackendV2 {
    public:
        virtual ~IPlotRenderBackendV2() = default;

        [[nodiscard]] virtual auto GetBackendId() const -> Str = 0;
        virtual auto Render(const FPlotFrameContextV2 &frame,
                            const FPlotDrawListV2 &drawList) -> bool = 0;
    };

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_PLOT2D_RENDER_BACKEND_V2_H
