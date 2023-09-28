//
// Created by joao on 28/09/23.
//

#include "Graph.h"

#include "Core/Tools/Animator.h"


auto Core::Graphics::Graph2D::getResolution() const -> Resolution        { return samples; }
auto Core::Graphics::Graph2D::setResolution(Resolution samples_) -> void { samples = samples_; }
const RectR& Core::Graphics::Graph2D::getRegion() const { return region; }
auto Core::Graphics::Graph2D::setLimits(RectR lims) -> void { region = lims; }

void Core::Graphics::Graph2D::set_xMin(Real val) { Animator::Add(region.xMin, val, animationTimeSeconds); }
void Core::Graphics::Graph2D::set_xMax(Real val) { Animator::Add(region.xMax, val, animationTimeSeconds); }
void Core::Graphics::Graph2D::set_yMin(Real val) { Animator::Add(region.yMin, val, animationTimeSeconds); }
void Core::Graphics::Graph2D::set_yMax(Real val) { Animator::Add(region.yMax, val, animationTimeSeconds); }
Real Core::Graphics::Graph2D::get_xMin() const { return region.xMin; }
Real Core::Graphics::Graph2D::get_xMax() const { return region.xMax; }
Real Core::Graphics::Graph2D::get_yMin() const { return region.yMin; }
Real Core::Graphics::Graph2D::get_yMax() const { return region.yMax; }

void Core::Graphics::Graph2D::setAnimationTime(Real value) { animationTimeSeconds = value; }

Real Core::Graphics::Graph2D::getAnimationTime() const { return animationTimeSeconds; }

void Core::Graphics::Graph2D::setHorizontalUnit(const Unit &hUnit) { baseHorizontalUnit = hUnit; }

void Core::Graphics::Graph2D::setVerticalUnit(const Unit &hUnit)   { baseVerticalUnit   = hUnit; }

auto Core::Graphics::Graph2D::getLastXHairPosition() const -> Point2D { return XHairLocation;
}

