#ifndef OUTPUTCONSOLEMONITOR_H
#define OUTPUTCONSOLEMONITOR_H

#include "OutputChannel.h"
#include <Common/Timer.h>

class OutputConsoleMonitor : public OutputChannel
{
private:
    Timer timer = Timer();
    const double maxT;
    char steppingChar;
public:
    OutputConsoleMonitor(const int n_steps, bool doCarrierReturn);

    virtual String getDescription() const {return "console monitor output";}

public:
    virtual bool notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation);

protected:
    virtual void _out(const OutputPacket &outputInfo);
};

#endif // OUTPUTCONSOLEMONITOR_H
