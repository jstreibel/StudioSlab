#ifndef STUDIOSLAB_RECORDING_RENDER_BACKEND_V2_H
#define STUDIOSLAB_RECORDING_RENDER_BACKEND_V2_H

#include "Graphics/Plot2D/V2/Plot2DRenderBackendV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    class FRecordingRenderBackendV2 final : public IPlotRenderBackendV2 {
        FPlotFrameContextV2 LastFrame;
        FPlotDrawListV2 LastDrawList;
        UIntBig RenderCallCount = 0;
        bool bResult = true;

    public:
        auto SetRenderResult(bool result) -> void;

        [[nodiscard]] auto GetBackendId() const -> Str override;
        auto Render(const FPlotFrameContextV2 &frame,
                    const FPlotDrawListV2 &drawList) -> bool override;

        [[nodiscard]] auto GetLastFrame() const -> const FPlotFrameContextV2 &;
        [[nodiscard]] auto GetLastDrawList() const -> const FPlotDrawListV2 &;
        [[nodiscard]] auto GetRenderCallCount() const -> UIntBig;
    };

    DefinePointers(FRecordingRenderBackendV2)

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_RECORDING_RENDER_BACKEND_V2_H
