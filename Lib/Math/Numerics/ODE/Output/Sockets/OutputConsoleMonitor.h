#ifndef OUTPUTCONSOLEMONITOR_H
#define OUTPUTCONSOLEMONITOR_H

#include "Math/Numerics/OutputChannel.h"
#include "Utils/Timer.h"


namespace Slab::Math {

    class OutputConsoleMonitor : public FOutputChannel {
    private:
        FTimer timer = FTimer();
        const CountType total_steps;

    protected:
        void HandleOutput(const OutputPacket &outputInfo) override;

    public:
        explicit OutputConsoleMonitor(CountType total_steps, CountType steps_interval=100);
        bool NotifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) override;
    };


}

#endif // OUTPUTCONSOLEMONITOR_H
