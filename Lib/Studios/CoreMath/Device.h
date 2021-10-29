#ifndef DEVICE_H
#define DEVICE_H

#include <Common/DeviceConfig.h>

#include <Studios/Controller/Interface/Interface.h>
#include <Studios/Controller/Interface/CommonParameters.h>

class Device : public Interface
{
public:
    Device();

    auto get_nThreads() const -> unsigned int {return *nThreads;}

    bool operator == (const int &RHS) const { return this->dev == RHS; }
    bool operator != (const int &RHS) const { return this->dev != RHS; }
    auto getDevice() const -> const device;

    void setup(CLVariablesMap vm) override;

private:
    device dev = device::CPU;
    IntegerParameter deviceChoice{0, "dev", "Device on which to run simulation.\n"
                                                       "\t0: CPU \n"
                                                       "\t1: GPU 0 \n"
                                                       "\t2: GPU 1 "};
    IntegerParameter nThreads{1, "threads,n", "Number of threads, in case of CPU usage."};
};

#endif // DEVICE_H
