//
// Created by joao on 29/05/23.
//

#include "PointSet.h"
#include "Math/Formalism/Categories.h"
#include <memory>

Math::PointSet::PointSet() : Space(2) { }

Math::PointSet::PointSet(const Math::Point2DVec& points) : PointSet() {
    for(const auto &p : points) addPoint(p);
}

Math::PointSet::PointSet(const Math::PointSet &pointSet)
: Space(2)
, points(pointSet.points)
, max(pointSet.max)
, min(pointSet.min)
{   }

Math::PointSet::Ptr Math::PointSet::New() {
    return std::make_shared<PointSet>(PointSet());
}

auto Math::PointSet::getMeasure() const -> const Measure {
    return {{max.x-min.x, max.y-min.y}};
}

auto Math::PointSet::getMax() const -> Math::Point2D {
    return max;
}

auto Math::PointSet::getMin() const -> Math::Point2D {
    return min;
}

void Math::PointSet::addPoint(const Math::Point2D &point) {
    if( points.empty() ){ min=point; max=point; }

    if     ( point.x > max.x ) max.x = point.x;
    else if( point.x < min.x ) min.x = point.x;

    if     ( point.y > max.y ) max.y = point.y;
    else if( point.y < min.y ) min.y = point.y;

    auto p = point;
    points.emplace_back(p);
}

void Math::PointSet::setPoints(Math::Point2DVec newPoints) {
    *this = PointSet(newPoints);
}

const Math::Point2DVec &Math::PointSet::getPoints() const { return points; }

void Math::PointSet::clear() {
    points.clear();
}

Count Math::PointSet::count() const { return points.size(); }








