#ifndef DEVICE_H
#define DEVICE_H

#include <Studios/Controller/Interface/Interface.h>
#include <Common/DeviceConfig.h>


class Device : public Interface
{
public:
    Device();

    auto get_nThreads() const -> unsigned int {return nThreads;}

    bool operator == (const int &RHS) const { return this->dev == RHS; }
    bool operator != (const int &RHS) const { return this->dev != RHS; }
    auto getDevice() const -> const device;

    void setup(CLVariablesMap vm) override;

private:
    device dev = device::CPU;
    unsigned int nThreads = 1;
};

#endif // DEVICE_H
