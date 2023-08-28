//
// Created by joao on 29/05/23.
//

#include "PointSet.h"
#include "Math/Formalism/Categories.h"
#include <memory>

Spaces::PointSet::PointSet() : Space(2) { }

Spaces::PointSet::PointSet(const Spaces::Point2DVec& points) : PointSet() {
    for(const auto &p : points) addPoint(p);
}

Spaces::PointSet::PointSet(const Spaces::PointSet &pointSet)
: Space(2)
, points(pointSet.points)
, max(pointSet.max)
, min(pointSet.min)
{   }

Spaces::PointSet::Ptr Spaces::PointSet::New() {
    return std::make_shared<PointSet>(PointSet());
}

auto Spaces::PointSet::getMeasure() const -> const Measure {
    return {{max.x-min.x, max.y-min.y}};
}

auto Spaces::PointSet::getMax() const -> Spaces::Point2D {
    return max;
}

auto Spaces::PointSet::getMin() const -> Spaces::Point2D {
    return min;
}

void Spaces::PointSet::addPoint(const Spaces::Point2D &point) {
    if( points.empty() ){ min=point; max=point; }

    if     ( point.x > max.x ) max.x = point.x;
    else if( point.x < min.x ) min.x = point.x;

    if     ( point.y > max.y ) max.y = point.y;
    else if( point.y < min.y ) min.y = point.y;

    auto p = point;
    points.emplace_back(p);
}

void Spaces::PointSet::setPoints(Spaces::Point2DVec newPoints) {
    this->points = newPoints;
}

const Spaces::Point2DVec &Spaces::PointSet::getPoints() const { return points; }

void Spaces::PointSet::clear() {
    points.clear();
}








