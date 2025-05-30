//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_POINTSET_H
#define STUDIOSLAB_POINTSET_H

#include "../Space.h"
#include "Math/Point.h"
#include "Utils/Pointer.h"


namespace Slab::Math {


    class PointSet : public Space {
        Point2DVec points;
        Point2D max={.0,.0}, min={.0,.0};

    public:
        PointSet();
        explicit
        PointSet(const Point2DVec& points);
        PointSet(const PointSet& pointSet);

        auto getMeasure() const -> const Measure override;

        auto getMax() const -> Point2D;
        auto getMin() const -> Point2D;

        Count count() const;

        void clear();
        void addPoint(Real x, Real y);
        void addPoint(const Point2D &point);
        void setPoints(Point2DVec points);

        const Point2DVec& getPoints() const;
        // Point2DVec& getPoints();
    };

    DefinePointers(PointSet)

}


#endif //STUDIOSLAB_POINTSET_H
