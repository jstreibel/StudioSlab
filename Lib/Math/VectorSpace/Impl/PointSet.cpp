//
// Created by joao on 29/05/23.
//

#include "PointSet.h"
#include "Math/Formalism/Categories.h"
#include <memory>

namespace Slab::Math {

    FPointSet::FPointSet() : Space(2) {}

    FPointSet::FPointSet(const Point2DVec &points) : FPointSet() {
        for (const auto &p: points) AddPoint(p);
    }

    FPointSet::FPointSet(const FPointSet &pointSet)
            : Space(2), points(pointSet.points), max(pointSet.max), min(pointSet.min) {}

    auto FPointSet::getMeasure() const -> const Measure {
        return {{max.x - min.x, max.y - min.y}};
    }

    auto FPointSet::getMax() const -> Point2D {
        return max;
    }

    auto FPointSet::getMin() const -> Point2D {
        return min;
    }

    void FPointSet::AddPoint(const Point2D &point) {
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

    void FPointSet::setPoints(Point2DVec newPoints) {
        *this = FPointSet(newPoints);
    }

    const
    Point2DVec &FPointSet::getPoints() const { return points; }

    Point2DVec& FPointSet::getPoints() { return points; }

    FPointSet operator+(const FPointSet& a, const FPointSet& b) {
        auto &pa = a.getPoints();
        auto &pb = b.getPoints();

        Point2DVec c(pa.size() + pb.size());
        std::copy(pa.begin(), pa.end(), c.begin());
        std::copy(pb.begin(), pb.end(), c.begin() + pa.size());

        return FPointSet(c);
    }

    // Point2DVec &PointSet::getPoints()       { return points; }

    void FPointSet::clear() { points.clear(); }

    CountType FPointSet::count() const { return points.size(); }

    void FPointSet::AddPoint(DevFloat x, DevFloat y) {
        AddPoint({x, y});
    }


}



