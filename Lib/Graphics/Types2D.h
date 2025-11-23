//
// Created by joao on 1/10/23.
//

#ifndef STUDIOSLAB_GRAPHICS_TYPES_H
#define STUDIOSLAB_GRAPHICS_TYPES_H

#include "Utils/Types.h"
#include "Utils/Numbers.h"

namespace Slab::Graphics {

    template<typename T>
    struct TRect {
        T xMin, xMax, yMin, yMax;

        T GetWidth() const {return xMax-xMin; }
        T GetHeight() const {return yMax - yMin; }

        T yCenter() const { return .5 * (yMax + yMin); }
        T xCenter() const { return .5 * (xMax + xMin); }

        bool doesHit(const T &x, const T &y) const { return x>=xMin && x<=xMax && y>=yMin && y<=yMax; }

        TRect operator +(const TRect& rhs) const { return { xMin+rhs.xMin, xMax+rhs.xMax, yMin+rhs.yMin, yMax+rhs.yMax }; }
        TRect operator *(const DevFloat& a) const { return {T(a*xMin), T(a*xMax), T(a*yMin), T(a*yMax)}; }

        bool operator==(const TRect &rhs) const {
            return Common::AreEqual(xMin, rhs.xMin) &&
                   Common::AreEqual(xMax, rhs.xMax) &&
                   Common::AreEqual(yMin, rhs.yMin) &&
                   Common::AreEqual(yMax, rhs.yMax);
        }

        bool operator!=(const TRect &rhs) const {
            return rhs != *this;
        }
    };

    typedef TRect<int> RectI;
    typedef TRect<DevFloat> RectR;

    DefinePointers(RectI)
    DefinePointers(RectR)

    struct Point2D {
        Point2D() = default;
        Point2D(const DevFloat x, const DevFloat y) : x(x), y(y) {}
        Point2D(const Int x, const Int y) : x(static_cast<DevFloat>(x)), y(static_cast<DevFloat>(y)) {}
        Point2D(const Point2D& p) = default;

        Point2D operator +  (const Point2D &p) const {return {x+p.x, y+p.y}; };

        void    operator += (const Point2D &p) { x+=p.x; y+=p.y; }
        void    operator -= (const Point2D &p) { x-=p.x; y-=p.y; }

        void    operator *= (const DevFloat &a) { x*=a; y*=a; }

        Point2D WithTranslation(const DevFloat dx, const DevFloat dy) const { return {x+dx, y+dy}; }

        DevFloat LengthSqr() const { return x * x + y * y;}
        DevFloat Length( ) const { return sqrt(LengthSqr());}

        DevFloat x, y;
    };

    // Point2D operator + (const Point2D &a, const Point2D &b);
    Point2D operator - (const Point2D &a, const Point2D &b);
    Point2D operator * (const DevFloat &a, const Point2D &p);
    Point2D operator * (const Point2D &p, const DevFloat &a);

    typedef Vector<Point2D> PointContainer;

}

#endif //STUDIOSLAB_GRAPHICS_TYPES_H
