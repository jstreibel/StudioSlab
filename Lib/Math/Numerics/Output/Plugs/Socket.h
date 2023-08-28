#ifndef OUTPUT_H
#define OUTPUT_H

#include "Math/Numerics/Output/Util/OutputPacket.h"
#include "Math/Numerics/SimConfig/NumericConfig.h"

#define INT_BASED 1
#define FLOAT_BASED 2

#define SHOULD_OUTPUT___MODE INT_BASED


namespace Numerics::OutputSystem {
    class Socket {
        int nextRecStep = -1;
        int intervalStepsBetweenOutputs; // Number of steps between recordings.

    protected:
        OutputPacket lastData;
        const NumericConfig &params;
        Str name, description;

        virtual auto handleOutput(const OutputPacket&) -> void = 0;

    public:
        explicit Socket(const NumericConfig &, Str name="", int nStepsInterval = 1, Str description="");
        virtual ~Socket() = default;

        std::shared_ptr<Socket> Ptr;

        virtual auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool;
        auto getDescription() const -> Str;
        auto getName() const -> Str;

        virtual auto computeNextRecStep(PosInt currStep) -> size_t;

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

        auto getLastSimTime()      -> Real;
        auto getnSteps()     const -> int;
        auto setnSteps(int nSteps) -> void;

        void output(const OutputPacket &outData);

    };
}


typedef std::vector<Numerics::OutputSystem::Socket*> OutputCollection;

#endif
