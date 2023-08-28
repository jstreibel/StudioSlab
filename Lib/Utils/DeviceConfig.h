#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H

#include "NativeTypes.h"

enum device {
    CPU = 1,
    GPU = 2,
    UNKNOWN = 3
};

#define GPU_DEBUG false

#if USE_CUDA == true
#define __forceinline__ __inline__ __attribute__((always_inline))
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/execution_policy.h>

#if GPU_DEBUG
typedef thrust::HostVector<floatt> DeviceVector;
#else
typedef thrust::device_vector<floatt> DeviceVector;
#endif
typedef thrust::host_vector<floatt> HostVector;
#else
class Dummy {};
typedef Dummy DeviceVector;
#endif

#endif // DEVICECONFIG_H
