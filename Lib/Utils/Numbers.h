//
// Created by joao on 23/09/23.
//

#ifndef STUDIOSLAB_NUMBERS_H
#define STUDIOSLAB_NUMBERS_H

#include "NativeTypes.h"

#include <complex>

const auto HUGE_NUMBER = ((unsigned long long)0)-1;

typedef floatt              Real;
typedef float               Real32;
typedef double              Real64;
typedef std::complex<Real>  Complex;

typedef unsigned char       Byte;
typedef unsigned char*      ByteData;
typedef Real32*             RealData;
typedef int                 Int;
typedef unsigned int        UInt;
typedef long long           IntBig;
typedef unsigned long       UIntBig;
typedef std::size_t         Count;
typedef std::size_t         Resolution;
typedef int_least64_t       BigInt;

union DoubleAccess {
    double val;
    char bytes[8];

    inline bool isPositive() const {
        return !((bytes[7] & 0x80) >> 7);

    }
};

struct Point2D {
    Point2D() = default;
    Point2D(Real x, Real y) : x(x), y(y) {}
    Point2D(const Point2D& p) = default;

    //Point2D operator * (const double a) const {
    //    return Point2D(x*a, y*a);
    //}
    //
    //Point2D operator - () const { return Point2D(-x,-y);}

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

#endif //STUDIOSLAB_NUMBERS_H
