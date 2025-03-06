#ifndef OUTPUTCONSOLEMONITOR_H
#define OUTPUTCONSOLEMONITOR_H

#include "Math/Numerics/Socket.h"
#include "Utils/Timer.h"


namespace Slab::Math {

    class OutputConsoleMonitor : public Socket {
    private:
        Timer timer = Timer();
        const Count total_steps;

    protected:
        void handleOutput(const OutputPacket &outputInfo) override;

    public:
        explicit OutputConsoleMonitor(Count total_steps, Count steps_interval=100);
        bool notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) override;
    };


}

#endif // OUTPUTCONSOLEMONITOR_H
