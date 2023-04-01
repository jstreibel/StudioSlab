#ifndef MATHUTIL_H
#define MATHUTIL_H

#include "Common/CoreNativeTypes.h"

#include <Common/Typedefs.h>

#ifndef __CUDACC__ // se estiver sendo compilado pelo nvcc
#include <boost/multiprecision/float128.hpp>
#define FLOAT128 boost::multiprecision::float128
#else
#define FLOAT128 double
#endif

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

inline Real delta(const Real x, const Real eps=1.e-5){
    const floatt delta = .5/eps;

    return ABS(x) <= eps ? delta : 0;
}

inline Real sqr(const Real x){ return x*x; }

#endif // MATHUTIL_H
