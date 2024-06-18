
//
// Created by joao on 29/08/2019.
//

#ifndef V_SHAPE_MATHTYPES_H
#define V_SHAPE_MATHTYPES_H

#include "Category.h"

#include "Utils/Utils.h"
#include "Math/Toolset/NativeFunctions.h"

namespace Slab::Math {

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

    class Real2D : public Category {
    public:
        Real2D() = default;

        Real2D(Real x, Real y) : x(x), y(y) {}

        Real x, y;

        Real2D operator-(const Real2D &p) const {
            return {x - p.x, y - p.y};
        }

        Real2D &operator-(const Real2D &p) {
            x -= p.x;
            y -= p.y;
            return *this;
        }

        friend Real2D operator*(const Real2D &p, const Real &r) {
            return {p.x * r, p.y * r};
        }

        friend Real2D operator+(const Real2D &p1, const Real2D &p2) {
            return {p1.x + p2.x, p1.y + p2.y};
        }

        Real norm() const {
            return sqrt(x * x + y * y);
        }
    };

    OStream &operator<<(OStream &stream, const Real2D &x);

    class Real3D : public Category {
    public:
        Real x, y, z;
    };

    class Rational : public Category {
    public:
        Rational();

        Rational(Int numerator, Int denominator);

        Rational(Real val, Real epsilon = 1e-9, Int maxIter = 1000);

        Str ToString() const override;

        Int numerator;
        Int denominator;
    };

    class Unit : public Category {
        Str sym;
        Real baseValue;
        unsigned maxNumerator;
        unsigned maxDenominator;
    public:

        Unit(Str symbol = "", Real val = 1.0, unsigned maxNumerator = 4, unsigned maxDenominator = 4);

        Unit(const Unit &unit);

        Real value() const;

        Str symbol() const;

        Str operator()(const Real &val, Count rounding) const;

        Str ToString() const override;

    };

}

#endif //V_SHAPE_MATHTYPES_H
