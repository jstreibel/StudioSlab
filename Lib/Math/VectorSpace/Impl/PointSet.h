//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_POINTSET_H
#define STUDIOSLAB_POINTSET_H

#include "../Space.h"
#include "Math/Point.h"
#include "Utils/Pointer.h"

namespace Slab::Math {


class FPointSet final : public Space {
    Point2DVec m_Points;
    Point2D max={.0,.0}, min={.0,.0};

public:
    FPointSet();
    explicit
    FPointSet(const Point2DVec& points);
    FPointSet(const FPointSet& pointSet);

    FPointSet Clone() const;

    auto GetMeasure() const -> const Measure override;

    auto GetMax() const -> Point2D;
    auto GetMin() const -> Point2D;

    CountType Count() const;

    void Clear();
    void AddPoint(DevFloat x, DevFloat y);
    FPointSet& AddPoint(const Point2D &point);
    void SetPoints(Point2DVec points);

    FPointSet TranslateConst(const Point2D &p) const;
    FPointSet& Translate(const Point2D &p);

    FPointSet operator*(Real64 a) const;
    FPointSet& operator*=(Real64 a);

    const Point2DVec& GetPoints() const;
    Point2DVec& GetPoints();
};

FPointSet operator + (const Real2D &a, const FPointSet &b);
FPointSet operator + (const FPointSet &a, const FPointSet &b);

DefinePointers(FPointSet)

}


#endif //STUDIOSLAB_POINTSET_H
