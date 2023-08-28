

#include "Core/Controller/Interface/InterfaceManager.h"
#include <omp.h>
#include "DeviceConfig.h"

#include "Utils/Utils.h"
#include "Utils/CUDAUtils.h"
#include "Core/Tools/Log.h"


DeviceConfig::DeviceConfig(bool doRegister) : InterfaceOwner("Device options", 10, doRegister)
{
    Log::Info() << "Device integration type is " << sizeof(Real)*8 << " bits." << Log::Flush;

    interface->addParameters(
    {
#if USE_CUDA
        deviceChoice,
#endif
        nThreads
    });
};


auto DeviceConfig::getDevice() const -> device {
    return dev;
}

void DeviceConfig::notifyCLArgsSetupFinished() {
    InterfaceOwner::notifyCLArgsSetupFinished();

#if USE_CUDA
    unsigned int dev_n = **deviceChoice;
#else
    unsigned int dev_n = 0;
#endif

    this->dev = dev_n == 0 ? CPU : (dev_n == 1 || dev_n == 2 ? GPU : UNKNOWN);

    if (dev == UNKNOWN) {
        throw (Str("Unkown device ") + std::to_string(dev_n) + Str(".")).c_str();
    } else if (dev == CPU) {
        omp_set_num_threads(**nThreads);

        Log::Info() << "Running on CPU @ " << *nThreads << " thread"
                    << (**nThreads > 1 ? "s." : ".") << Log::Flush;

    } else if (dev == GPU) {
#if USE_CUDA
        int devCount;
        cudaError err;

        cew(cudaGetDeviceCount(&devCount));
        cudaDeviceProp props;
        cudaGetDeviceProperties(&props, (int)dev_n);

        cew(cudaSetDevice(dev_n - 1));

        Log::Info() << "Running on GPU " << dev_n << "/" << devCount << ", " << Str(props.name) << Log::Flush;

        if (**nThreads > 1) {
            Log::Attention() << "Ignoring n_threads argument (using GPU)." << Log::Flush;
            *nThreads = 1;
        }
#else
        throw "Code was not compiled with GPU support. And this exception should never "
              "in a logical universe have happened.";
#endif
    }
}


