#ifndef DEVICE_H
#define DEVICE_H

#include <Common/DeviceConfig.h>

#include <Base/Controller/Interface/InterfaceOwner.h>
#include <Base/Controller/Interface/CommonParameters.h>

class Device : public InterfaceOwner
{
    auto notifyCLArgsSetupFinished() -> void override;

    device dev = device::CPU;
    IntegerParameter::Ptr deviceChoice = IntegerParameter::New(0, "dev", "Device on which to run simulation.\n"
                                                                         "\t0: CPU \n"
                                                                         "\t1: GPU 0 \n"
                                                                         "\t2: GPU 1 ");
    IntegerParameter::Ptr nThreads = IntegerParameter::New(1, "threads,n", "Number of threads, in case of CPU usage.");

public:
    Device();

    auto get_nThreads() const -> unsigned int {return **nThreads;}

    bool operator == (const int &RHS) const { return this->dev == RHS; }
    bool operator != (const int &RHS) const { return this->dev != RHS; }
    auto getDevice() const -> const device;

};

#endif // DEVICE_H
