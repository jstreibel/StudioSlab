#ifndef MATHUTIL_H
#define MATHUTIL_H

//#include "nr3.h"
//#include <type_traits>

#ifndef __CUDACC__ // se estiver sendo compilado pelo nvcc
#include <boost/multiprecision/float128.hpp>
#define FLOAT128 boost::multiprecision::float128
#else
#define FLOAT128 double
#endif

const double e = 2.718281828459045235360287471352;
const double pi = 3.14159265358979323846;
const double pi_1 = 1.0/pi;

#define USE_NRVECTORS false

#define FLOAT64 double
#define FLOAT32 float

typedef FLOAT64 floatt;

#if USE_NRVECTORS
typedef NRvector<floatt> VecFloat;
#else
#include <vector>
typedef std::vector<floatt> VecFloat;
#endif
typedef const VecFloat VecFloat_I;
typedef VecFloat VecFloat_O;
typedef VecFloat VecFloat_IO;



inline bool isPOT(const unsigned long n){
    return (n != 0) && ((n & (n - 1)) == 0);
}

template<class T>
inline T SIGN(const T &a)
    {return a>0 ? 1 : (a<0 ? -1 : 0);}

template<class T>
inline T ABS(const T &a)
    {return a>=0 ? a : -a;}

inline bool isEqual(const floatt a, const floatt b,
                    const floatt eps = 1.e-3)
{
    return a>b-eps && a<b+eps;
}

inline floatt UnitStep(const floatt x, const floatt eps=0){
    return x < eps ? 0 : 1;
}

inline floatt delta(const floatt x, const floatt eps=1.e-5){
    const floatt delta = .5/eps;

    return ABS(x) <= eps ? delta : 0;
}

inline floatt sqr(const floatt x){ return x*x; }

#endif // MATHUTIL_H
