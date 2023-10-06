//
// Created by joao on 1/10/23.
//

#ifndef STUDIOSLAB_GRAPHICS_TYPES_H
#define STUDIOSLAB_GRAPHICS_TYPES_H

#include "Utils/Types.h"
#include "Utils/Numbers.h"

namespace Graphics {

    template<typename T>
    struct Rect {
        T xMin, xMax, yMin, yMax;

        T width() const {return xMax-xMin; }
        T height() const {return yMax - yMin; }

        T yCenter() const { return .5 * (yMax + yMin); }
        T xCenter() const { return .5 * (xMax + xMin); }

        bool doesHit(const T &x, const T &y) const { return x>=xMin && x<=xMax && y>=yMin && y<=yMax; }
    };

    typedef Rect<int> RectI;
    typedef Rect<Real> RectR;


    struct Point2D {
        Point2D() = default;
        Point2D(Real x, Real y) : x(x), y(y) {}
        Point2D(const Point2D& p) = default;

        Point2D operator +  (const Point2D &p) {return {x+p.x, y+p.y}; };
        void    operator += (const Point2D &p) { x+=p.x; y+=p.y; }
        void    operator -= (const Point2D &p) { x-=p.x; y-=p.y; }

        inline Real lengthSqr() const { return x * x + y * y;}
        inline Real length( ) const { return sqrt(lengthSqr());}

        Real x, y;
    };

    Point2D operator + (const Point2D &a, const Point2D &b);
    Point2D operator - (const Point2D &a, const Point2D &b);
    Point2D operator * (const Real &a, const Point2D &p);

    typedef std::vector<Point2D> PointContainer;

    struct Point3D { float x, y, z; };

    typedef Point3D Vector3D;

}

#endif //STUDIOSLAB_GRAPHICS_TYPES_H
