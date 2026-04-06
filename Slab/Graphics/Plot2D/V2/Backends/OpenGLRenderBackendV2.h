#ifndef STUDIOSLAB_OPENGL_RENDER_BACKEND_V2_H
#define STUDIOSLAB_OPENGL_RENDER_BACKEND_V2_H

#include "Graphics/Plot2D/V2/Plot2DRenderBackendV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    class FOpenGLRenderBackendV2 final : public IPlotRenderBackendV2 {
        bool bRenderLegend = true;

    public:
        [[nodiscard]] auto GetBackendId() const -> Str override;
        auto Render(const FPlotFrameContextV2 &frame,
                    const FPlotDrawListV2 &drawList) -> bool override;

        auto SetRenderLegend(bool renderLegend) -> void;
        [[nodiscard]] auto GetRenderLegend() const -> bool;
    };

    DefinePointers(FOpenGLRenderBackendV2)

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_OPENGL_RENDER_BACKEND_V2_H
