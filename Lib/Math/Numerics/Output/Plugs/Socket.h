#ifndef OUTPUT_H
#define OUTPUT_H

#include "Math/Numerics/Output/Util/OutputPacket.h"

#define INT_BASED 1
#define FLOAT_BASED 2

#define SHOULD_OUTPUT___MODE INT_BASED


namespace Slab::Math {
    class Socket {
        int nextRecStep = -1;
        int intervalStepsBetweenOutputs; // Number of steps between recordings.

    protected:
        OutputPacket lastPacket;
        Str name, description;

        virtual auto handleOutput(const OutputPacket &) -> void = 0;

    public:
        explicit Socket(Str name = "", int nStepsInterval = 1, Str description = "");

        virtual ~Socket();

        std::shared_ptr<Socket> Ptr;

        virtual auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool;

        auto getDescription() const -> Str;

        auto getName() const -> Str;

        virtual auto computeNextRecStep(UInt currStep) -> size_t;

        /*!
         * This function allows child classes to give additional info on whether info should be output, or completely
         * override this information. It is basically used by the class NumericalIntegration, which needs to know when to output
         * stuff.
         * @param t The Real-valued time at which the requirer wants to know if output should be generated.
         * @param timestep The timestemp at which the requirer wants to know if output should be generated. Should match 't'
         * above.
         * @return
         */
        virtual auto shouldOutput(Real t, long unsigned timestep) -> bool;

        auto getLastSimTime() -> Real;

        auto getnSteps() const -> int;

        virtual auto setnSteps(int nSteps) -> void;

        void output(const OutputPacket &outData);

    };

    DefinePointers(Socket)

    typedef Vector<Socket *> OutputCollection;

}


#endif
