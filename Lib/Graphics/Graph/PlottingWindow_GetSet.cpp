//
// Created by joao on 28/09/23.
//

#include "PlottingWindow.h"

#include "Core/Backend/Modules/Animator/Animator.h"


namespace Slab {

    auto Graphics::PlottingWindow::getRegion() const -> const Graphics::PlottingRegion2D &
    { return region; }
    auto Graphics::PlottingWindow::getRegion() -> Graphics::PlottingRegion2D &
    { return region; }

    void Graphics::PlottingWindow::setAnimationTime(Real value) { animationTimeSeconds = value; }

    Real Graphics::PlottingWindow::getAnimationTime() const { return animationTimeSeconds; }

    auto Graphics::PlottingWindow::getLastXHairPosition() const -> Point2D {
        auto vpRect = getViewport();
        fix mouseLocal = getMouseViewportCoord();

        return FromViewportToSpaceCoord(mouseLocal, region.getRect(), vpRect);
    }


}