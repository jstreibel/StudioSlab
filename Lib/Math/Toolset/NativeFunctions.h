#ifndef MATHUTIL_H
#define MATHUTIL_H

#include "Utils/NativeTypes.h"

#include <Utils/Types.h>

//#ifndef __CUDACC__ // se estiver sendo compilado pelo nvcc
//#include <boost/multiprecision/float128.hpp>
//#define FLOAT128 boost::multiprecision::float128
//#else
//#define FLOAT128 double
//#endif


namespace Slab::Math {


    inline bool isPOT(const unsigned long n) {
        return (n != 0) && ((n & (n - 1)) == 0);
    }

    template<class T>
    inline T SIGN(const T &a) { return a > 0 ? 1 : (a < 0 ? -1 : 0); }

/*
const auto RealSize = sizeof(Real);
inline Real SIGN(const Real &val){
    union RealBits {
        Real val;
        char bytes[RealSize];

        inline bool isPositive() const {
            return !((bytes[RealSize-1] & 0x80) >> (RealSize-1));

        }
    };

    asdfas
}
*/

    template<class T>
    inline T ABS(const T &a) { return a >= 0 ? a : -a; }

/*
union DoubleBits {
    double value;
    uint64_t bits;
};

double absoluteValue(double value) {
    DoubleBits db;
    db.value = value;
    db.bits &= ~(1ULL << 63); // Clear the sign bit.
    return db.value;
}
*/


    inline bool isEqual(const Real a, const Real b,
                        const Real eps = 1.e-3) {
        return a > b - eps && a < b + eps;
    }

    inline Real UnitStep(const Real x, const Real eps = 0) {
        return x < eps ? 0 : 1;
    }

    DEVICES inline auto deltaGauss(Real x, Real eps) -> Real {
        return 1. / sqrt(4 * M_PI * eps) * exp(-x * x / (4 * eps));
    }

    DEVICES inline Real deltaTri(const Real x, const Real eps = 1.e-5) {
        const Real invEps = 1. / eps;
        const Real absx = std::abs(x);

        if (absx < eps) return invEps * (1. - invEps * absx);

        return 0.;
    }

    DEVICES inline auto deltaRect(Real x, Real eps) -> Real {
        if (std::abs(x) < eps) return .5 / eps;

        return 0.;
    }

    DEVICES inline Real logAbs(Real val, Real eps) {
        const auto sign = (val > .0 ? 1.0 : -1.0);
        return log(std::abs(val) / eps + 1) * sign;
    }

    DEVICES inline Real logAbs_inv(Real val, Real eps) {
        const auto sign = (val > .0 ? 1.0 : -1.0);
        return eps * (exp(std::abs(val)) - 1.0) * sign;
    }

    inline Real sqr(const Real x) { return x * x; }

}

#endif // MATHUTIL_H
