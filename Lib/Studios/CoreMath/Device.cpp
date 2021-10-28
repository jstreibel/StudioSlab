#include <Studios/Controller/Interface/InterfaceManager.h>
#include "Device.h"

#include "Studios/Controller/Interface/CommonParameters.h"

Device::Device() : Interface("Device options")
{
    addParameters(
    {
#if USE_CUDA
        new IntegerParameter(dev, "dev", "Device on which to run simulation.\n"
                                         "\t0: CPU \n"
                                         "\t1: GPU 0 \n"
                                         "\t2: GPU 1 "),
#endif
                new IntegerParameter(nThreads, "threads", "Number of threads, in case of "
                                                          "CPU usage. Can be anything from 1-16.")});
};


auto Device::getDevice() const -> const device {
    return dev;
}

void Device::setup(CLVariablesMap vm) {
    Interface::setup(vm);

    this->nThreads = vm["threads"].as<unsigned int>();
#if USE_CUDA
    unsigned int dev_n = vm["dev"].as<unsigned int>();
#else
    unsigned int dev_n = 0;
#endif

    this->dev = dev_n == 0 ? CPU : (dev_n == 1 || dev_n == 2 ? GPU : UNKNOWN);

    if (dev == UNKNOWN) {
        throw (String("Unkown device ") + std::to_string(dev_n) + String(".")).c_str();
    } else if (dev == CPU) {
        std::cout << "CPU " << nThreads << " thread" << (nThreads > 1 ? "s.\n" : ".\n");
    } else if (dev == GPU) {
#if USE_CUDA
        int devCount;
        cudaGetDeviceCount(&devCount);
        std::cout << "GPU " << dev_n << "/" << devCount << std::endl;

        cudaError err = cudaSetDevice(dev_n - 1);
        if (err != cudaError::cudaSuccess) throw (String("CUDA error ") + std::to_string(err)).c_str();

        if (this->nThreads > 1) {
            std::cout << "Ignoring n_threads argument (using GPU)." << std::endl;
            this->nThreads = 1;
        }
#else
        throw "Code was not compiled with GPU support.";
#endif
    }
}


