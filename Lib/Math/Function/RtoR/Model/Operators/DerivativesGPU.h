#ifndef DERIVATIVESGPU_CUH
#define DERIVATIVESGPU_CUH

#include "Utils/DeviceConfig.h"
#include "Utils/TypesGPU.h"
#include "Utils/Types.h"

#include <cuda_runtime_api.h>
#include <cuda_runtime.h>
#include <cuda.h>

namespace Slab::Math::RtoR {

    __host__ DeviceVector &d2dx2_DANIEL(const DeviceVector &in,
                                        DeviceVector &out,
                                        Real h, size_t N);

    __host__ DeviceVector &d2dx2(const DeviceVector &in,
                                 DeviceVector &out,
                                 Real h, size_t N, bool periodic);

}

#endif // DERIVATIVESGPU_CUH
