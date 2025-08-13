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
    inline T Abs(const T &a) { return a > 0 ? a : -a; }

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


    inline bool isEqual(const DevFloat a, const DevFloat b,
                        const DevFloat eps = 1.e-3) {
        return a > b - eps && a < b + eps;
    }

    inline DevFloat UnitStep(const DevFloat x, const DevFloat eps = 0) {
        return x < eps ? 0.0 : 1.0;
    }

    DEVICES inline auto deltaGauss(DevFloat x, DevFloat eps) -> DevFloat {
        return 1. / sqrt(4 * M_PI * eps) * exp(-x * x / (4 * eps));
    }

    DEVICES inline DevFloat deltaTri(const DevFloat x, const DevFloat eps = 1.e-5) {
        const DevFloat invEps = 1. / eps;
        const DevFloat absx = std::abs(x);

        if (absx < eps) return invEps * (1. - invEps * absx);

        return 0.;
    }

    DEVICES inline auto deltaRect(DevFloat x, DevFloat eps) -> DevFloat {
        if (std::abs(x) < eps) return .5 / eps;

        return 0.;
    }

    DEVICES inline DevFloat logAbs(DevFloat val, DevFloat eps) {
        const auto sign = (val > .0 ? 1.0 : -1.0);
        return log(std::abs(val) / eps + 1) * sign;
    }

    DEVICES inline DevFloat logAbs_inv(DevFloat val, DevFloat eps) {
        const auto sign = (val > .0 ? 1.0 : -1.0);
        return eps * (exp(std::abs(val)) - 1.0) * sign;
    }

    inline DevFloat sqr(const DevFloat x) { return x * x; }

}

#endif // MATHUTIL_H
