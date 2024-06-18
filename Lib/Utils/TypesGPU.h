//
// Created by joao on 6/15/24.
//

#ifndef STUDIOSLAB_TYPESGPU_H
#define STUDIOSLAB_TYPESGPU_H

#include "NativeTypes.h"

namespace Slab {
    enum Device {
        CPU = 1,
        GPU = 2,
        UNKNOWN = 3
    };
}

#if USE_CUDA == true
// #define __forceinline__ __inline__ __attribute__((always_inline))
#include <cuda_runtime.h>
#include <thrust/host_vector.h>
#ifdef __CUDACC__
#include <thrust/device_vector.h>
#endif
#include <thrust/execution_policy.h>

namespace Slab::CUDA {
#ifdef __CUDACC__
    typedef thrust::device_vector<floatt> DeviceVector;
#endif
    typedef thrust::host_vector<floatt> HostVector;

}

#else  // USE_CUDA
namespace Slab::CUDA {
    typedef class Dummy {} DeviceVector;
}
#endif // USE_CUDA

#endif //STUDIOSLAB_TYPESGPU_H
