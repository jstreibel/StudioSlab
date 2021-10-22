#ifndef DEVICECONFIG_H
#define DEVICECONFIG_H

#if USE_CUDA == true
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/execution_policy.h>
#endif

enum device {
    CPU = 1,
    GPU = 2,
    UNKNOWN = 3
};

#define GPU_DEBUG false

typedef double Real;

#if USE_CUDA == true
#if GPU_DEBUG
typedef thrust::HostVector<Real> DeviceVector;
#else
typedef thrust::device_vector<Real> DeviceVector;
#endif
typedef thrust::host_vector<Real> HostVector;
#else
class Dummy {};
typedef Dummy DeviceVector;
#endif

#endif // DEVICECONFIG_H
