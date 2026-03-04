#include "RecordingRenderBackendV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    auto FRecordingRenderBackendV2::SetRenderResult(const bool result) -> void {
        bResult = result;
    }

    auto FRecordingRenderBackendV2::GetBackendId() const -> Str {
        return "plot2d.v2.recording";
    }

    auto FRecordingRenderBackendV2::Render(const FPlotFrameContextV2 &frame,
                                           const FPlotDrawListV2 &drawList) -> bool {
        LastFrame = frame;
        LastDrawList = drawList;
        ++RenderCallCount;
        return bResult;
    }

    auto FRecordingRenderBackendV2::GetLastFrame() const -> const FPlotFrameContextV2 & {
        return LastFrame;
    }

    auto FRecordingRenderBackendV2::GetLastDrawList() const -> const FPlotDrawListV2 & {
        return LastDrawList;
    }

    auto FRecordingRenderBackendV2::GetRenderCallCount() const -> UIntBig {
        return RenderCallCount;
    }

} // namespace Slab::Graphics::Plot2D::V2
