

#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include <omp.h>
#include "DeviceConfig.h"

#include "Utils/Utils.h"
#include "Utils/CUDAUtils.h"
#include "Core/Tools/Log.h"


namespace Slab::Math {
    using namespace Slab::Core;

    DeviceConfig::DeviceConfig(bool doRegister) : CLInterfaceOwner("Device options", 10, doRegister)
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


    auto DeviceConfig::getDevice() const -> Device {
        return dev;
    }

    void DeviceConfig::notifyCLArgsSetupFinished() {
        CLInterfaceOwner::notifyCLArgsSetupFinished();

    #if USE_CUDA
        unsigned int dev_n = **deviceChoice;
    #else
        unsigned int dev_n = 0;
    #endif

        this->dev = dev_n == 0 ? CPU : (dev_n == 1 || dev_n == 2 ? GPU : UNKNOWN);

        if (dev == UNKNOWN) {
            throw Exception(Str("Unknown device ") + std::to_string(dev_n) + Str("."));
        } else if (dev == Device::CPU) {
            #if OMP_SUPPORT == true
            omp_set_num_threads(**nThreads);
            #endif

            Log::Info() << "Running on CPU @ " << *nThreads << " thread"
                        << (**nThreads > 1 ? "s." : ".") << Log::Flush;

        } else if (dev == Device::GPU) {
    #if USE_CUDA
            setupForThread();
    #else
            throw Exception("Code was not compiled with GPU support. And this exception should never "
                  "in a logical universe have happened.");
    #endif
        }
    }

    void DeviceConfig::setupForThread() {
        #if USE_CUDA
        unsigned int dev_n = **deviceChoice;
        if(dev_n==0) return;
        int devCount;
        cudaError err;

        Slab::CUDA::cew(cudaGetDeviceCount(&devCount));
        cudaDeviceProp props{};
        cudaGetDeviceProperties(&props, (int)dev_n - 1);

        Slab::CUDA::cew(  cudaSetDevice((int)dev_n - 1));
        // Slab::CUDA::cew(cudaSetDevice(2));

        Log::Info() << "Running on GPU " << dev_n << "/" << devCount << ", " << Str(props.name) << Log::Flush;

        if (**nThreads > 1) {
            Log::Attention() << "Ignoring n_threads argument (using GPU)." << Log::Flush;
            *nThreads = 1;
        }
        #endif
    }

    void DeviceConfig::notifyAllCLArgsSetupFinished() {

        CLInterfaceListener::notifyAllCLArgsSetupFinished();
    }




}