#ifndef DERIVATIVESGPU_CUH
#define DERIVATIVESGPU_CUH

#include "Common/DeviceConfig.h"
#include "Common/Types.h"

__host__ DeviceVector &d2dx2(const DeviceVector &in,
                             DeviceVector &out,
                             const Real h, const size_t N, const size_t M);

#endif // DERIVATIVESGPU_CUH
