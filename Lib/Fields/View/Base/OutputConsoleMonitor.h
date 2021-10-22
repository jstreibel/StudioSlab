#ifndef OUTPUTCONSOLEMONITOR_H
#define OUTPUTCONSOLEMONITOR_H

#include "OutputChannel.h"
#include <Studios/Util/timer.h>

class OutputConsoleMonitor : public OutputChannel
{
private:
    Timer timer;
    const double maxT;
    char steppingChar;
public:
    OutputConsoleMonitor(const int n_steps, bool doCarrierReturn);

    virtual String description() const {return "console monitor output";}

public:
    virtual bool notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation);

protected:
    virtual void _out(const OutputPacket &outputInfo);
};

#endif // OUTPUTCONSOLEMONITOR_H
