#ifndef OUTPUTCONSOLEMONITOR_H
#define OUTPUTCONSOLEMONITOR_H

#include "Phys/Numerics/Output/Plugs/Socket.h"
#include <Common/Timer.h>

class OutputConsoleMonitor : public Numerics::OutputSystem::Socket
{
private:
    Timer timer = Timer();
    const Real maxT;
public:
    OutputConsoleMonitor(const NumericParams &params, const int n_steps);

public:
    virtual bool notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) override;

protected:
    virtual void _out(const OutputPacket &outputInfo) override;
};

#endif // OUTPUTCONSOLEMONITOR_H
