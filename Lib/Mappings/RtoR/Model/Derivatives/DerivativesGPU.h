#ifndef DERIVATIVESGPU_CUH
#define DERIVATIVESGPU_CUH

#include "Common/DeviceConfig.h"
#include "Common/Types.h"

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
