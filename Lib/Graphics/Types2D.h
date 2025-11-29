//
// Created by joao on 1/10/23.
//

#ifndef STUDIOSLAB_GRAPHICS_TYPES_H
#define STUDIOSLAB_GRAPHICS_TYPES_H

#include "Math/Formalism/Categories.h"
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

    struct FPoint2D {
        FPoint2D() = default;
        FPoint2D(const DevFloat x, const DevFloat y) : x(x), y(y) {}
        FPoint2D(const Int x, const Int y) : x(static_cast<DevFloat>(x)), y(static_cast<DevFloat>(y)) {}
        FPoint2D(const FPoint2D& p) = default;
        explicit FPoint2D(const Math::Real2D& p) : x(p.x), y(p.y) {}

        FPoint2D operator +  (const FPoint2D &p) const {return {x+p.x, y+p.y}; }

        void    operator += (const FPoint2D &p) { x+=p.x; y+=p.y; }
        void    operator -= (const FPoint2D &p) { x-=p.x; y-=p.y; }

        void    operator *= (const DevFloat &a) { x*=a; y*=a; }

        FPoint2D WithTranslation(const DevFloat dx, const DevFloat dy) const { return {x+dx, y+dy}; }
        FPoint2D WithRotation(DevFloat AngleRad) const;

        Math::Real2D ToReal2D() const { return {x, y}; }

        DevFloat LengthSqr() const { return x * x + y * y;}
        DevFloat Length( ) const { return sqrt(LengthSqr());}

        DevFloat x, y;
    };

    // Point2D operator + (const Point2D &a, const Point2D &b);
    FPoint2D operator - (const FPoint2D &a, const FPoint2D &b);
    FPoint2D operator * (const DevFloat &a, const FPoint2D &p);
    FPoint2D operator * (const FPoint2D &p, const DevFloat &a);

    typedef Vector<FPoint2D> PointContainer;

}

#endif //STUDIOSLAB_GRAPHICS_TYPES_H
