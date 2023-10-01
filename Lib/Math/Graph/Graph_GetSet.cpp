//
// Created by joao on 28/09/23.
//

#include "Graph.h"

#include "Core/Tools/Animator.h"


auto Graphics::Graph2D::getResolution() const -> Resolution        { return samples; }
auto Graphics::Graph2D::setResolution(Resolution samples_) -> void { samples = samples_; }
const Graphics::RectR& Graphics::Graph2D::getRegion() const { return region; }
auto Graphics::Graph2D::setLimits(RectR lims) -> void { region = lims; }

void Graphics::Graph2D::set_xMin(Real val) { Core::Animator::Add(region.xMin, val, animationTimeSeconds); }
void Graphics::Graph2D::set_xMax(Real val) { Core::Animator::Add(region.xMax, val, animationTimeSeconds); }
void Graphics::Graph2D::set_yMin(Real val) { Core::Animator::Add(region.yMin, val, animationTimeSeconds); }
void Graphics::Graph2D::set_yMax(Real val) { Core::Animator::Add(region.yMax, val, animationTimeSeconds); }
Real Graphics::Graph2D::get_xMin() const { return region.xMin; }
Real Graphics::Graph2D::get_xMax() const { return region.xMax; }
Real Graphics::Graph2D::get_yMin() const { return region.yMin; }
Real Graphics::Graph2D::get_yMax() const { return region.yMax; }

void Graphics::Graph2D::setAnimationTime(Real value) { animationTimeSeconds = value; }

Real Graphics::Graph2D::getAnimationTime() const { return animationTimeSeconds; }

void Graphics::Graph2D::setHorizontalUnit(const Unit &unit) {
    baseHorizontalUnit = unit;
    axisArtist.setHorizontalUnit(unit);
}

void Graphics::Graph2D::setVerticalUnit(const Unit &unit)   {
    baseVerticalUnit= unit;
    axisArtist.setVerticalUnit(unit);
}

auto Graphics::Graph2D::getLastXHairPosition() const -> Point2D { return XHairLocation;
}

