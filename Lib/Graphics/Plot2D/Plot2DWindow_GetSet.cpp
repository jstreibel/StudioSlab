//
// Created by joao on 28/09/23.
//

#include "Plot2DWindow.h"

namespace Slab {

    auto Graphics::FPlot2DWindow::GetLastXHairPosition() const -> FPoint2D {
        auto vpRect = GetViewport();

        auto Mouse = GetMouseState();
        fix MouseLocal = FromPlatformWindowToViewportCoords({Mouse->x, Mouse->y});;

        return FromViewportToSpaceCoord(MouseLocal, Region.getRect(), vpRect);
    }
    auto Graphics::FPlot2DWindow::GetRegion() const -> const Graphics::PlottingRegion2D &
    { return Region; }

    auto Graphics::FPlot2DWindow::GetRegion() -> Graphics::PlottingRegion2D &
    { return Region; }

    void Graphics::FPlot2DWindow::SetRegion(const PlottingRegion2D& NewRegion) {
        this->Region = NewRegion;
    }

    void Graphics::FPlot2DWindow::SetAnimationTime(const float value) { AnimationTimeSeconds = value; }

    float Graphics::FPlot2DWindow::GetAnimationTime() const { return AnimationTimeSeconds; }


}