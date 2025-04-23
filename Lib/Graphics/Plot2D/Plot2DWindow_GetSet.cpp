//
// Created by joao on 28/09/23.
//

#include "Plot2DWindow.h"

namespace Slab {

    auto Graphics::Plot2DWindow::getRegion() const -> const Graphics::PlottingRegion2D &
    { return region; }
    auto Graphics::Plot2DWindow::getRegion() -> Graphics::PlottingRegion2D &
    { return region; }

    void Graphics::Plot2DWindow::setAnimationTime(Real value) { animationTimeSeconds = value; }

    Real Graphics::Plot2DWindow::getAnimationTime() const { return animationTimeSeconds; }

    auto Graphics::Plot2DWindow::getLastXHairPosition() const -> Point2D {
        auto vpRect = getViewport();
        fix mouseLocal = getMouseViewportCoord();

        return FromViewportToSpaceCoord(mouseLocal, region.getRect(), vpRect);
    }


}