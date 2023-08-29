//
// Created by joao on 10/25/21.
//

#ifndef STUDIOSLAB_TYPES_H
#define STUDIOSLAB_TYPES_H

#include "DeviceConfig.h"
#include "NativeTypes.h"
#include "STDLibInclude.h"

typedef floatt              Real;

typedef unsigned char       Byte;
typedef unsigned char*      ByteData;
typedef long                Int;
typedef unsigned long       PosInt;
typedef std::size_t         Count;
typedef std::size_t         Resolution;
typedef int_least64_t       BigInt;

#define let     auto
#define OUT auto&
#define IN  const auto&
#define GET auto&
#define fix     const auto
#define look    const auto *

fix HUGE_NUMBER = ((unsigned long long)0)-1;

typedef std::vector<Real>   RealVector;
typedef const RealVector      VecFloat_I;
typedef RealVector            VecFloat_O;
typedef RealVector            VecFloat_IO;

typedef std::vector<int>    IntVector;

template<class T1, class T2>
struct Pair {
    T1 first;
    T2 second;
};

typedef Pair<int, int>      IntPair;
typedef std::pair<const class DiscreteSpace*, const class DiscreteSpace*> DiscreteSpacePair;


template<typename T>
std::shared_ptr<T> DummyPtr(T &instance) {
    return std::shared_ptr<T>(&instance, [](T*){});
}


template<class T>
typename std::vector<T>     Vector;
typedef std::ostream        OStream;
typedef std::string         Str;
typedef std::stringstream   StringStream;
typedef std::vector<Str> StrVector;

#include <vector>
#include <iostream>
#include <cmath>

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

typedef std::vector<Point2D> PointContainer;


#endif //STUDIOSLAB_TYPES_H
