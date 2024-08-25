#ifndef OUTPUTCONSOLEMONITOR_H
#define OUTPUTCONSOLEMONITOR_H

#include "Math/Numerics/Output/Plugs/Socket.h"
#include <Utils/Timer.h>


namespace Slab::Math {

    class OutputConsoleMonitor : public Socket {
    private:
        Timer timer = Timer();
        const Real maxT;

    public:
        explicit OutputConsoleMonitor(const NumericConfig &params);

    public:
        bool notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) override;

    protected:
        virtual void handleOutput(const OutputPacket &outputInfo) override;
    };


}

#endif // OUTPUTCONSOLEMONITOR_H
