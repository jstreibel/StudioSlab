#ifndef OUTPUT_H
#define OUTPUT_H

#include "Phys/Numerics/Output/Util/OutputPacket.h"
#include "Phys/Numerics/Program/NumericParams.h"

#define INT_BASED 1
#define FLOAT_BASED 2

#define SHOULD_OUTPUT___MODE INT_BASED

namespace Numerics {
    namespace OutputSystem {
        class Socket {
            int nextRecStep = -1;
            int nSteps; // Number of steps between recordings.

        protected:
            OutputPacket lastData;
            const NumericParams &params;
            Str name, description;

            virtual auto _out(const OutputPacket&) -> void = 0;

        public:
            /**
             * Constructor for output Socket.
             * @param numericParams The simulation numeric parameters.
             * @param name The socket name.
             * @param nStepsInterval Interval, in simulation steps, to output to socket. If <1, then it will only be called when simulation is finished.
             * @param description The socket descriptino.
             */
            explicit Socket(const NumericParams &numericParams, Str name="", int nStepsInterval = 1, Str description="");
            virtual ~Socket() = default;

            std::shared_ptr<Socket> Ptr;

            virtual auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool;;
            auto getDescription() const -> Str;
            auto getName() const -> Str;

            virtual auto computeNextRecStep() -> size_t;

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
}

typedef std::vector<Numerics::OutputSystem::Socket*> OutputCollection;

#endif
