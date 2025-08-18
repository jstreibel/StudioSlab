#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include "OutputChannel.h"


namespace Slab::Math {

    class FOutputManager final {
    protected:
        Vector<FOutputChannel_ptr > Outputs;

    public:
        explicit FOutputManager(CountType MaxSteps=0);

        ~FOutputManager();

        /**
         * This parameter comes in by reference because copying it can be very heavy. OTOH it is treated as if it is
         * allocated on the HEAP.
         *
         * @param InfoVolatile the information to output.
         * @param ForceOutput
         */
        void Output(const OutputPacket &InfoVolatile, bool ForceOutput = false) const;

        void NotifyIntegrationFinished(const OutputPacket &info) const;

        /****** QUERY ******/
        auto ComputeNStepsToNextOutput(UInt CurrStep) -> UInt;

        /****** INPUT/OUTPUT ******/
        void AddOutputChannel(TPointer<FOutputChannel> out);
        void SetMaxSteps(UInt MaxSteps);

    private:
        size_t MaxSteps;


    };


}

#endif // OUTPUTMANAGER_H
