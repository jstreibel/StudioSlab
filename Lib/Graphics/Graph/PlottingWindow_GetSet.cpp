//
// Created by joao on 28/09/23.
//

#include "PlottingWindow.h"

#include "Graphics/Animator.h"


const Graphics::RectR& Graphics::PlottingWindow::getRegion() const { return region; }
auto Graphics::PlottingWindow::setLimits(RectR lims) -> void { region = lims; }

void Graphics::PlottingWindow::set_xMin(Real val) { Core::Animator::Add(region.xMin, val, animationTimeSeconds); }
void Graphics::PlottingWindow::set_xMax(Real val) { Core::Animator::Add(region.xMax, val, animationTimeSeconds); }
void Graphics::PlottingWindow::set_yMin(Real val) { Core::Animator::Add(region.yMin, val, animationTimeSeconds); }
void Graphics::PlottingWindow::set_yMax(Real val) { Core::Animator::Add(region.yMax, val, animationTimeSeconds); }
Real Graphics::PlottingWindow::get_xMin() const { return region.xMin; }
Real Graphics::PlottingWindow::get_xMax() const { return region.xMax; }
Real Graphics::PlottingWindow::get_yMin() const { return region.yMin; }
Real Graphics::PlottingWindow::get_yMax() const { return region.yMax; }

void Graphics::PlottingWindow::setAnimationTime(Real value) { animationTimeSeconds = value; }

Real Graphics::PlottingWindow::getAnimationTime() const { return animationTimeSeconds; }

auto Graphics::PlottingWindow::getLastXHairPosition() const -> Point2D {
    auto vpRect = getViewport();
    fix mouseLocal = getMouseViewportCoord();

    return FromViewportToSpaceCoord(mouseLocal, region, vpRect);
}

