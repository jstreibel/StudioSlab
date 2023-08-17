//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_POINTSET_H
#define STUDIOSLAB_POINTSET_H

#include "../Space.h"
#include "../Point.h"


namespace Spaces {


    class PointSet : public Space {
        Point2DVec points;
        Point2D max={.0,.0}, min={.0,.0};

    public:
        PointSet();
        PointSet(const Point2DVec points);
        PointSet(const PointSet& pointSet);

        typedef std::shared_ptr<PointSet> Ptr;
        static Ptr New();

        auto getMeasure() const -> const Measure override;

        auto getMax() const -> Point2D;
        auto getMin() const -> Point2D;

        void clear();
        void addPoint(const Point2D &point);
        void setPoints(Point2DVec points);

        const Point2DVec& getPoints() const;

    };



}


#endif //STUDIOSLAB_POINTSET_H
