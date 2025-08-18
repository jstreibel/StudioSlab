#ifndef OUTPUTCONSOLEMONITOR_H
#define OUTPUTCONSOLEMONITOR_H

#include "Math/Numerics/Socket.h"
#include "Utils/Timer.h"


namespace Slab::Math {

    class OutputConsoleMonitor : public Socket {
    private:
        FTimer timer = FTimer();
        const CountType total_steps;

    protected:
        void HandleOutput(const OutputPacket &outputInfo) override;

    public:
        explicit OutputConsoleMonitor(CountType total_steps, CountType steps_interval=100);
        bool notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) override;
    };


}

#endif // OUTPUTCONSOLEMONITOR_H
