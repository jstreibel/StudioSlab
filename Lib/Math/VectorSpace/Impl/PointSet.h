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
    Point2DVec points;
    Point2D max={.0,.0}, min={.0,.0};

public:
    FPointSet();
    explicit
    FPointSet(const Point2DVec& points);
    FPointSet(const FPointSet& pointSet);

    auto getMeasure() const -> const Measure override;

    auto getMax() const -> Point2D;
    auto getMin() const -> Point2D;

    CountType count() const;

    void clear();
    void AddPoint(DevFloat x, DevFloat y);
    void AddPoint(const Point2D &point);
    void setPoints(Point2DVec points);

    const Point2DVec& getPoints() const;
    Point2DVec& getPoints();
};

FPointSet operator + (const FPointSet &a, const FPointSet &b);

DefinePointers(FPointSet)

}


#endif //STUDIOSLAB_POINTSET_H
