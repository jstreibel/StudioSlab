#include <Studios/Controller/Interface/InterfaceManager.h>
#include "Device.h"



Device::Device() : Interface("Device options")
{
    addParameters(
    {
#if USE_CUDA
        &deviceChoice,
#endif
        &nThreads});
};


auto Device::getDevice() const -> const device {
    return dev;
}

void Device::setup(CLVariablesMap vm) {
    Interface::setup(vm);

#if USE_CUDA
    unsigned int dev_n = *deviceChoice;
#else
    unsigned int dev_n = 0;
#endif

    this->dev = dev_n == 0 ? CPU : (dev_n == 1 || dev_n == 2 ? GPU : UNKNOWN);

    if (dev == UNKNOWN) {
        throw (String("Unkown device ") + std::to_string(dev_n) + String(".")).c_str();
    } else if (dev == CPU) {
        std::cout << "CPU " << *nThreads << " thread" << (*nThreads > 1 ? "s.\n" : ".\n");
    } else if (dev == GPU) {
#if USE_CUDA
        int devCount;
        cudaGetDeviceCount(&devCount);
        std::cout << "GPU " << dev_n << "/" << devCount << std::endl;

        cudaError err = cudaSetDevice(dev_n - 1);
        if (err != cudaError::cudaSuccess) throw (String("CUDA error ") + std::to_string(err)).c_str();

        if (*nThreads > 1) {
            std::cout << "Ignoring n_threads argument (using GPU)." << std::endl;
            *nThreads = 1;
        }
#else
        throw "Code was not compiled with GPU support. And this exception should have never "
              "in a logical universe have happened.";
#endif
    }
}

