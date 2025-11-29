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
            : Space(2), m_Points(pointSet.m_Points), max(pointSet.max), min(pointSet.min) {}

    auto FPointSet::GetMeasure() const -> const Measure {
        return {{max.x - min.x, max.y - min.y}};
    }

    auto FPointSet::GetMax() const -> Point2D {
        return max;
    }

    auto FPointSet::GetMin() const -> Point2D {
        return min;
    }

    FPointSet& FPointSet::AddPoint(const Point2D &point) {
        if (m_Points.empty()) {
            min = point;
            max = point;
        }

        if (point.x > max.x) max.x = point.x;
        else if (point.x < min.x) min.x = point.x;

        if (point.y > max.y) max.y = point.y;
        else if (point.y < min.y) min.y = point.y;

        auto p = point;
        m_Points.emplace_back(p);

        return *this;
    }

    void FPointSet::SetPoints(Point2DVec newPoints) {
        *this = FPointSet(newPoints);
    }

    FPointSet FPointSet::TranslateConst(const Point2D& p) const {
        FPointSet Result;

        for (const auto &pt: m_Points) Result.AddPoint(pt + p);

        return Result;
    }

    FPointSet& FPointSet::Translate(const Point2D& p) {
        for (auto &pt: m_Points) pt+=p;

        return *this;
    }

    FPointSet& FPointSet::Rotate(const Real64& AngleRad) {
        fix c = cos(AngleRad);
        fix s = sin(AngleRad);

        for (auto &pt: m_Points) {
            fix x = pt.x;
            pt.x = c * x - s * pt.y;
            pt.y = s * x + c * pt.y;
        }

        return *this;
    }

    FPointSet FPointSet::operator*(const Real64 a) const {
        FPointSet Result;

        for (const auto &p: m_Points) Result.AddPoint(p * a);

        return Result;
    }

    FPointSet& FPointSet::operator*=(Real64 a) {
        for (auto &p: m_Points) p*=a;

        return *this;
    }

    const
    Point2DVec &FPointSet::GetPoints() const { return m_Points; }

    Point2DVec& FPointSet::GetPoints() { return m_Points; }

    FPointSet operator+(const Real2D& a, const FPointSet& b) {
        FPointSet Result{};

        Result.AddPoint(a);
        for (fix& Pt : b.GetPoints()) Result.AddPoint(Pt);

        return Result;
    }

    FPointSet operator+(const FPointSet& a, const FPointSet& b) {
        auto &pa = a.GetPoints();
        auto &pb = b.GetPoints();

        Point2DVec c(pa.size() + pb.size());
        std::copy(pa.begin(), pa.end(), c.begin());
        std::copy(pb.begin(), pb.end(), c.begin() + pa.size());

        return FPointSet(c);
    }

    // Point2DVec &PointSet::getPoints()       { return points; }

    void FPointSet::Clear() { m_Points.clear(); }

    CountType FPointSet::Count() const { return m_Points.size(); }

    void FPointSet::AddPoint(DevFloat x, DevFloat y) {
        AddPoint({x, y});
    }


}



