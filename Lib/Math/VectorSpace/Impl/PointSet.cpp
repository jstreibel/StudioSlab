//
// Created by joao on 29/05/23.
//

#include "PointSet.h"
#include "Math/Formalism/Categories.h"
#include <memory>

namespace Slab::Math {

    PointSet::PointSet() : Space(2) {}

    PointSet::PointSet(const Point2DVec &points) : PointSet() {
        for (const auto &p: points) AddPoint(p);
    }

    PointSet::PointSet(const PointSet &pointSet)
            : Space(2), points(pointSet.points), max(pointSet.max), min(pointSet.min) {}

    auto PointSet::getMeasure() const -> const Measure {
        return {{max.x - min.x, max.y - min.y}};
    }

    auto PointSet::getMax() const -> Point2D {
        return max;
    }

    auto PointSet::getMin() const -> Point2D {
        return min;
    }

    void PointSet::AddPoint(const Point2D &point) {
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

    const
    Point2DVec &PointSet::getPoints() const { return points; }

    Point2DVec& PointSet::getPoints() { return points; }

    PointSet operator+(const PointSet& a, const PointSet& b) {
        auto &pa = a.getPoints();
        auto &pb = b.getPoints();

        Point2DVec c(pa.size() + pb.size());
        std::copy(pa.begin(), pa.end(), c.begin());
        std::copy(pb.begin(), pb.end(), c.begin() + pa.size());

        return PointSet(c);
    }

    // Point2DVec &PointSet::getPoints()       { return points; }

    void PointSet::clear() { points.clear(); }

    CountType PointSet::count() const { return points.size(); }

    void PointSet::AddPoint(DevFloat x, DevFloat y) {
        AddPoint({x, y});
    }


}



