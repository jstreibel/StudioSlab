#ifndef DERIVATIVESGPU_CUH
#define DERIVATIVESGPU_CUH

#include "Lib/Util/device-config.h"

#include <cuda_runtime_api.h>
#include <cuda_runtime.h>
#include <cuda.h>

__host__ DeviceVector &d2dx2(const DeviceVector &in,
                             DeviceVector &out,
                             const double h, const size_t N);

__host__ DeviceVector &d2dx2(const DeviceVector &in,
                             DeviceVector &out,
                             const double h, const size_t N, const size_t M);

#endif // DERIVATIVESGPU_CUH
