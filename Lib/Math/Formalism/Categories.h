
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
    static const DevFloat NaN = std::numeric_limits<DevFloat>::quiet_NaN();
    static const DevFloat EPS = 1e-12;
//Uint proto_nan[2]={0xffffffff, 0x7fffffff};
//double NaN = *( double* )proto_nan;
//Doub NaN = sqrt(-1.);

    typedef Pair<DevFloat, DevFloat> RealPair;

    class Real1D : public Category {
    public:
        DevFloat x;
    };

    class Real2D final /* : public Category <-- deriving adds 8bytes to size. Don`t change, as it breaks AB */ {
    public:
        DevFloat x, y;

        Real2D() = default;

        Real2D(DevFloat x, DevFloat y) : x(x), y(y) {}

        Real2D operator-(const Real2D &p) const {
            return {x - p.x, y - p.y};
        }

        Real2D &operator-(const Real2D &p) {
            x -= p.x;
            y -= p.y;
            return *this;
        }

        Real2D &operator*=(const Real64 &a) {
            x *= a;
            y *= a;

            return *this;
        }

        Real2D &operator+=(const Real2D &p) {
            x += p.x;
            y += p.y;

            return *this;
        }

        friend Real2D operator*(const Real2D &p, const DevFloat &r) {
            return {p.x * r, p.y * r};
        }

        friend Real2D operator+(const Real2D &p1, const Real2D &p2) {
            return {p1.x + p2.x, p1.y + p2.y};
        }

        [[nodiscard]] DevFloat norm() const {
            return sqrt(x * x + y * y);
        }

        Real2D operator/(const Real64 mass) const {
            return {x / mass, y / mass};
        }

        Real2D operator-() const;
    };

    OStream &operator<<(OStream &stream, const Real2D &x);

    class Real3D : public Category {
    public:
        DevFloat x, y, z;
    };

    class Rational final : public Category {
    public:
        Rational();

        Rational(Int numerator, Int denominator);

        explicit Rational(DevFloat val, DevFloat epsilon = 1e-9, Int maxIter = 1000);

        Str ToString() const override;

        Int numerator = 0;
        Int denominator = 1;
    };

    class Unit : public Category {
        Str sym;
        DevFloat baseValue;
        unsigned maxNumerator;
        unsigned maxDenominator;
    public:

        explicit Unit(Str symbol = "", DevFloat val = 1.0, unsigned maxNumerator = 4, unsigned maxDenominator = 4);

        Unit(const Unit &unit);

        DevFloat value() const;

        Str symbol() const;

        Str operator()(const DevFloat &val, CountType rounding) const;

        Str ToString() const override;

    };

}

#endif //V_SHAPE_MATHTYPES_H
