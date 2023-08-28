#ifndef OUTPUTCONSOLEMONITOR_H
#define OUTPUTCONSOLEMONITOR_H

#include "Math/Numerics/Output/Plugs/Socket.h"
#include <Common/Timer.h>

class OutputConsoleMonitor : public Numerics::OutputSystem::Socket
{
private:
    Timer timer = Timer();
    const Real maxT;

public:
    OutputConsoleMonitor(const NumericConfig &params, Count n_steps_between_calls);

public:
    bool notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) override;

protected:
    virtual void handleOutput(const OutputPacket &outputInfo);
};

#endif // OUTPUTCONSOLEMONITOR_H
