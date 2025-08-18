#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include "Socket.h"


namespace Slab::Math {

    class FOutputManager final {
    protected:
        Vector<Socket_ptr > outputs;

    public:
        explicit FOutputManager(CountType max_steps);

        ~FOutputManager();

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
        void addOutputChannel(TPointer<Socket> out);


    private:
        const size_t maxSteps;


    };


}

#endif // OUTPUTMANAGER_H
