//
// Created by joao on 29/05/23.
//

#include "PointSet.h"
#include "Math/Formalism/Categories.h"
#include <memory>

namespace Slab::Math {

    PointSet::PointSet() : Space(2) {}

    PointSet::PointSet(const Point2DVec &points) : PointSet() {
        for (const auto &p: points) addPoint(p);
    }

    PointSet::PointSet(const PointSet &pointSet)
            : Space(2), points(pointSet.points), max(pointSet.max), min(pointSet.min) {}

    PointSet::Ptr PointSet::New() {
        return std::make_shared<PointSet>(PointSet());
    }

    auto PointSet::getMeasure() const -> const Measure {
        return {{max.x - min.x, max.y - min.y}};
    }

    auto PointSet::getMax() const -> Point2D {
        return max;
    }

    auto PointSet::getMin() const -> Point2D {
        return min;
    }

    void PointSet::addPoint(const Point2D &point) {
        if (points.empty()) {
            min = point;
            max = point;
        }

        if (point.x > max.x) max.x = point.x;
        else if (point.x < min.x) min.x = point.x;

        if (point.y > max.y) max.y = point.y;
        else if (point.y < min.y) min.y = point.y;

        auto p = point;
        points.emplace_back(p);
    }

    void PointSet::setPoints(Point2DVec newPoints) {
        *this = PointSet(newPoints);
    }

    const Point2DVec &PointSet::getPoints() const { return points; }

    void PointSet::clear() {
        points.clear();
    }

    Count PointSet::count() const { return points.size(); }


}



