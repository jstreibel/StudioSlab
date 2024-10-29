#ifndef OUTPUTCONSOLEMONITOR_H
#define OUTPUTCONSOLEMONITOR_H

#include "Socket.h"
#include "Utils/Timer.h"


namespace Slab::Math {

    class OutputConsoleMonitor : public Socket {
    private:
        Timer timer = Timer();
        const Real max_t;
        const Count total_steps;

    public:
        explicit OutputConsoleMonitor(Count total_steps, Real max_t);

    public:
        bool notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) override;

    protected:
        virtual void handleOutput(const OutputPacket &outputInfo) override;
    };


}

#endif // OUTPUTCONSOLEMONITOR_H
