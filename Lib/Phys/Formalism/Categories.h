//
// Created by joao on 29/08/2019.
//

#ifndef V_SHAPE_MATHTYPES_H
#define V_SHAPE_MATHTYPES_H

#include "Category.h"

#include "Common/Utils.h"
#include "Phys/Toolset/NativeFunctions.h"

// NaN: uncomment one of the following 3 methods of defining a global NaN
// you can test by verifying that (NaN != NaN) is true
static const Real NaN = std::numeric_limits<Real>::quiet_NaN();
//Uint proto_nan[2]={0xffffffff, 0x7fffffff};
//double NaN = *( double* )proto_nan;
//Doub NaN = sqrt(-1.);

typedef Pair<Real, Real> RealPair;

class Real1D : public Category {
public:
    Real x;
};

class Real2D : public Category{
public:
    Real2D() = default;
    Real2D(Real x, Real y) : x(x), y(y) {}
    Real x, y;

    Real2D& operator-(const Real2D& p) {
        x -= p.x;
        y -= p.y;
        return *this;
    }

    friend Real2D operator*(const Real2D& p, const Real& r) {
        return {p.x*r, p.y*r};
    }

    friend Real2D operator+(const Real2D& p1, const Real2D& p2) {
        return {p1.x+p2.x, p1.y+p2.y};
    }

    float norm() {
        return sqrt(x*x + y*y);
    }
};

std::ostream& operator<< (std::ostream& stream, const Real2D& x);

class Real3D : public Category{
public:
    Real x, y, z;
};

#endif //V_SHAPE_MATHTYPES_H
