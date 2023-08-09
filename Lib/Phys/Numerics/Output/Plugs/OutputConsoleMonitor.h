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
    OutputConsoleMonitor(const NumericParams &params, const int n_steps_between_calls);

    virtual Str getDescription() const {return "console monitor output";}

public:
    virtual bool notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation);

protected:
    virtual void _out(const OutputPacket &outputInfo);
};

#endif // OUTPUTCONSOLEMONITOR_H
