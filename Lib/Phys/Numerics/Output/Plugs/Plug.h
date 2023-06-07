#ifndef OUTPUT_H
#define OUTPUT_H

#include "Phys/Numerics/Output/Util/OutputPacket.h"

#define INT_BASED 1
#define FLOAT_BASED 2

#define SHOULD_OUTPUT___MODE INT_BASED

namespace Numerics {
    namespace OutputSystem {
        class Plug {

            virtual void _out(const OutputPacket &outputPacket) = 0;

        public:
            virtual ~Plug() = default;

            std::shared_ptr<Plug> Ptr;

            virtual auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool;;
            auto getDescription() const -> String;
            auto getName() const -> String;

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


        protected:

            int nStepsBetweenRecordings; // numero de passos entre uma gravacao e outra.
            OutputPacket lastData;


            String name, description;
        public:

            explicit Plug(String name="", int nStepsInterval = 1, String description="");

            auto getLastSimTime() -> Real;
            auto getNSteps() const -> int;

            void output(const OutputPacket &outData);


        };
    }
}

typedef std::vector<Numerics::OutputSystem::Plug*> OutputCollection;

#endif
