#ifndef OUTPUT_H
#define OUTPUT_H

#include "Math/Numerics/ODE/Output/Util/OutputPacket.h"

#define INT_BASED 1
#define FLOAT_BASED 2

#define SHOULD_OUTPUT___MODE INT_BASED


namespace Slab::Math {
    class FOutputChannel {
        int NextRecStep = -1;
        int IntervalStepsBetweenOutputs; // Number of steps between recordings.

    protected:
        FOutputPacket LastPacket;
        Str Name, Description;

        virtual auto HandleOutput(const FOutputPacket &) -> void = 0;

    public:
        explicit FOutputChannel(Str name = "", int nStepsInterval = 1, Str Description = "");

        virtual ~FOutputChannel();

        // std::shared_ptr<FOutputChannel> Ptr;

        virtual auto NotifyIntegrationHasFinished(const FOutputPacket &TheVeryLastOutputInformation) -> bool;

        [[nodiscard]] auto GetDescription() const -> Str;

        [[nodiscard]] auto GetName() const -> Str;

        virtual auto ComputeNextRecStep(UInt currStep) -> size_t;

        /*!
         * This function allows child classes to give additional info on whether info should be output, or completely
         * override this information. It is basically used by the class NumericalIntegration, which needs to know when to output
         * stuff.
         * @param timestep The timestemp at which the requirer wants to know if output should be generated. Should match 't'
         * above.
         * @return
         */
        virtual auto ShouldOutput(long unsigned timestep) -> bool;

        [[nodiscard]] auto Get_nSteps() const -> int;

        virtual auto Set_nSteps(int nSteps) -> void;

        void Output(const FOutputPacket &outData);

    };

    DefinePointers(FOutputChannel)

    typedef Vector<FOutputChannel *> OutputCollection;

}


#endif
