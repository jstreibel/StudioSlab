#ifndef DEVICE_H
#define DEVICE_H

#include "Common/DeviceConfig.h"

#include "Core/Controller/Interface/InterfaceOwner.h"
#include "Core/Controller/Interface/CommonParameters.h"

class DeviceConfig : public InterfaceOwner
{
    auto notifyCLArgsSetupFinished() -> void override;

    device dev = device::CPU;
    IntegerParameter::Ptr deviceChoice = IntegerParameter::New(1, "dev", "Device on which to run simulation.\n"
                                                                         "\t0: CPU \n"
                                                                         "\t1: GPU 0 \n"
                                                                         "\t2: GPU 1 ");
    IntegerParameter::Ptr nThreads = IntegerParameter::New(1, "th,threads", "Number of threads, in case of CPU usage.");

public:
    DeviceConfig(bool doRegister=true);

    auto get_nThreads() const -> unsigned int {return **nThreads;}

    bool operator == (const int &RHS) const { return this->dev == RHS; }
    bool operator != (const int &RHS) const { return this->dev != RHS; }
    auto getDevice() const -> device;



};

#endif // DEVICE_H
