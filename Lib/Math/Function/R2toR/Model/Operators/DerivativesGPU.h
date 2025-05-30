#ifndef DERIVATIVESGPU_CUH
#define DERIVATIVESGPU_CUH

#include "Utils/DeviceConfig.h"
#include "Utils/TypesGPU.h"
#include "Utils/Types.h"

namespace Slab::Math::R2toR {

    __host__ DeviceVector &d2dx2(const DeviceVector &in,
                                 DeviceVector &out,
                                 const Real h, const size_t N, const size_t M);

}

#endif // DERIVATIVESGPU_CUH
