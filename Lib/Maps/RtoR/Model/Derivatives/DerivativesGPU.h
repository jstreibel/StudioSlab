#ifndef DERIVATIVESGPU_CUH
#define DERIVATIVESGPU_CUH

#include "Utils/DeviceConfig.h"
#include "Utils/Types.h"

#include <cuda_runtime_api.h>
#include <cuda_runtime.h>
#include <cuda.h>

__host__ DeviceVector &d2dx2_DANIEL(const DeviceVector &in,
                             DeviceVector &out,
                             const Real h, const size_t N);

__host__ DeviceVector &d2dx2(const DeviceVector &in,
                             DeviceVector &out,
                             const Real h, const size_t N);


#endif // DERIVATIVESGPU_CUH
