#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include "Math/Numerics/ODE/Output/Sockets/Socket.h"


namespace Slab::Math {

    class OutputManager final {
    protected:
        Vector<Socket_ptr > outputs;

    public:
        explicit OutputManager(Count max_steps);

        ~OutputManager();

        /**
         * This parameter comes in by reference because copying it can be very heavy. OTOH it is treated as if it is
         * allocated on the HEAP.
         *
         * @param infoVolatile the information to output.
         */
        void output(OutputPacket &infoVolatile, bool force = false);

        void notifyIntegrationFinished(const OutputPacket &info);

        /****** QUERY ******/
        auto computeNStepsToNextOutput(UInt currStep) -> UInt;

        /****** INPUT/OUTPUT ******/
        void addOutputChannel(Pointer<Socket> out);


    private:
        const size_t maxSteps;


    };


}

#endif // OUTPUTMANAGER_H
