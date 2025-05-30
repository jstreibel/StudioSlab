#ifndef DEVICE_H
#define DEVICE_H

#include "Utils/DeviceConfig.h"
#include "Utils/TypesGPU.h"
#include "Core/Controller/CommandLine/CLInterfaceOwner.h"
#include "Core/Controller/CommandLine/CommonCLParameters.h"


namespace Slab::Math {
    using namespace Slab::Core;

    class DeviceConfig : public CLInterfaceOwner {
        auto notifyCLArgsSetupFinished() -> void override;

    public:
        auto notifyAllCLArgsSetupFinished() -> void override;

    private:
        Device dev = Device::CPU;
        IntegerParameter::Ptr deviceChoice = IntegerParameter::New(1, "dev", "Device on which to run simulation.\n"
                                                                             "\t0: CPU \n"
                                                                             "\t1: GPU 0 \n"
                                                                             "\t2: GPU 1 ");
        IntegerParameter::Ptr nThreads = IntegerParameter::New(1, "th,threads",
                                                               "Number of threads, in case of CPU usage.");

    public:
        explicit DeviceConfig(bool doRegister = true);

        void setupForThread();

        auto get_nThreads() const -> unsigned int { return **nThreads; }

        bool operator==(const int &RHS) const { return this->dev == RHS; }

        bool operator!=(const int &RHS) const { return this->dev != RHS; }

        auto getDevice() const -> Device;


    };

}

#endif // DEVICE_H
