#ifndef NBODY_CUH
#define NBODY_CUH

#include <cstdlib>
#include "cuda_runtime.h"

#if 0
typedef float DevFloat;
typedef float2 Real2;
#define Sqrt(x) sqrtf(x)
#else
typedef double DevFloat;
typedef double2 Real2;
#define Sqrt(x) sqrt(x)
#endif

Real getBoxHalfSide();
unsigned int getN();
Real getR();

__host__ void initDevice(Real2 *host_r, Real2 *host_p);
__host__ Real2 *fetch_rFromDevice(Real2 *rHost);
__host__ Real2 *fetch_pFromDevice(Real2 *pHost);

__host__ void dampSpeed(DevFloat factor);

const char *getIntMethodStr();

void step();




#endif // !NBODY_CUH

