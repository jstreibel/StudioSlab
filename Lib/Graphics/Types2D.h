//
// Created by joao on 1/10/23.
//

#ifndef STUDIOSLAB_GRAPHICS_TYPES_H
#define STUDIOSLAB_GRAPHICS_TYPES_H

#include "Utils/Types.h"
#include "Utils/Numbers.h"

namespace Slab::Graphics {

    template<typename T>
    struct Rect {
        T xMin, xMax, yMin, yMax;

        T width() const {return xMax-xMin; }
        T height() const {return yMax - yMin; }

        T yCenter() const { return .5 * (yMax + yMin); }
        T xCenter() const { return .5 * (xMax + xMin); }

        bool doesHit(const T &x, const T &y) const { return x>=xMin && x<=xMax && y>=yMin && y<=yMax; }

        bool operator==(const Rect &rhs) const {
            return Common::AreEqual(xMin, rhs.xMin) &&
                   Common::AreEqual(xMax, rhs.xMax) &&
                   Common::AreEqual(yMin, rhs.yMin) &&
                   Common::AreEqual(yMax, rhs.yMax);
        }

        bool operator!=(const Rect &rhs) const {
            return !(rhs == *this);
        }
    };

    typedef Rect<int> RectI;
    typedef Rect<Real> RectR;

    DefinePointer(RectI)
    DefinePointer(RectR)

    struct Point2D {
        Point2D() = default;
        Point2D(Real x, Real y) : x(x), y(y) {}
        Point2D(const Point2D& p) = default;

        Point2D operator +  (const Point2D &p) const {return {x+p.x, y+p.y}; };

        void    operator += (const Point2D &p) { x+=p.x; y+=p.y; }
        void    operator -= (const Point2D &p) { x-=p.x; y-=p.y; }

        void    operator *= (const Real &a) { x*=a; y*=a; }


        inline Real lengthSqr() const { return x * x + y * y;}
        inline Real length( ) const { return sqrt(lengthSqr());}

        Real x, y;
    };

    // Point2D operator + (const Point2D &a, const Point2D &b);
    Point2D operator - (const Point2D &a, const Point2D &b);
    Point2D operator * (const Real &a, const Point2D &p);

    typedef Vector<Point2D> PointContainer;

}

#endif //STUDIOSLAB_GRAPHICS_TYPES_H
