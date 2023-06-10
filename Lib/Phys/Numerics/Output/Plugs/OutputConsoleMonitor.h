#ifndef OUTPUTCONSOLEMONITOR_H
#define OUTPUTCONSOLEMONITOR_H

#include "Phys/Numerics/Output/Plugs/Plug.h"
#include <Common/Timer.h>

class OutputConsoleMonitor : public Numerics::OutputSystem::Plug
{
private:
    Timer timer = Timer();
    const Real maxT;
public:
    OutputConsoleMonitor(const int n_steps);

    virtual String getDescription() const {return "console monitor output";}

public:
    virtual bool notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation);

protected:
    virtual void _out(const OutputPacket &outputInfo);
};

#endif // OUTPUTCONSOLEMONITOR_H
