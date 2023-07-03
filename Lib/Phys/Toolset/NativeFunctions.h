#ifndef MATHUTIL_H
#define MATHUTIL_H

#include "Common/NativeTypes.h"

#include <Common/Types.h>

//#ifndef __CUDACC__ // se estiver sendo compilado pelo nvcc
//#include <boost/multiprecision/float128.hpp>
//#define FLOAT128 boost::multiprecision::float128
//#else
//#define FLOAT128 double
//#endif

inline bool isPOT(const unsigned long n){
    return (n != 0) && ((n & (n - 1)) == 0);
}

template<class T>
inline T SIGN(const T &a)
    {return a>0 ? 1 : (a<0 ? -1 : 0);}

template<class T>
inline T ABS(const T &a)
    {return a>=0 ? a : -a;}

inline bool isEqual(const Real a, const Real b,
                    const Real eps = 1.e-3)
{
    return a>b-eps && a<b+eps;
}

inline Real UnitStep(const Real x, const Real eps=0){
    return x < eps ? 0 : 1;
}

__device__ __host__ inline auto deltaGauss(Real x, Real eps) -> Real {
    return 1. / sqrt(4 * M_PI * eps) * exp(-x * x / (4 * eps));
}

__device__ __host__ inline Real deltaTri(const Real x, const Real eps=1.e-5){
    const Real invEps = 1. / eps;
    const Real absx = std::abs(x);

    if (absx < eps) return invEps * (1. - invEps * absx);

    return 0.;
}

__device__ __host__ inline auto deltaRect(Real x, Real eps) -> Real {
    if (std::abs(x) < eps) return .5 / eps;

    return 0.;
}

inline Real sqr(const Real x){ return x*x; }

#endif // MATHUTIL_H
