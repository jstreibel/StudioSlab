//
// Created by joao on 10/8/21.
//

#include "OutputChannel.h"
#include "Core/Tools/Log.h"


namespace Slab::Math {

    FOutputChannel::FOutputChannel(Str name, int nStepsInterval, Str Description)
            : IntervalStepsBetweenOutputs(nStepsInterval), Name(name), Description(Description),
              NextRecStep(1) {}

    auto FOutputChannel::Get_nSteps() const -> int { return IntervalStepsBetweenOutputs; }

    auto FOutputChannel::Set_nSteps(int n) -> void {
        IntervalStepsBetweenOutputs = n >= 1 ? n : 1;
    }

    auto FOutputChannel::ComputeNextRecStep(UInt currStep) -> size_t {
        if (NextRecStep > currStep)
            return NextRecStep;

        fix n = (DevFloat) IntervalStepsBetweenOutputs;
        fix m = (DevFloat) (currStep + 1);

        NextRecStep = (int) (n * std::ceil(m / n));

        return NextRecStep;
    }

    auto FOutputChannel::ShouldOutput(const long unsigned timestep) -> bool {
#if SHOULD_OUTPUT___MODE == INT_BASED
        return !(timestep % IntervalStepsBetweenOutputs);
#elif SHOULD_OUTPUT___MODE == FLOAT_BASED
        return abs(T-lastT) > abs(recDT);
#endif

        throw "Boundary not implemented.";
    }

    void FOutputChannel::Output(const OutputPacket &outData) {
        HandleOutput(outData);
        LastPacket = outData;
    }

    auto FOutputChannel::NotifyIntegrationHasFinished(const OutputPacket &TheVeryLastOutputInformation) -> bool {
        LastPacket = TheVeryLastOutputInformation;
        return true;
    }

    auto FOutputChannel::GetDescription() const -> Str { return Description; }

    auto FOutputChannel::GetName() const -> Str { return Name; }

    FOutputChannel::~FOutputChannel() {
        Core::Log::Info() << "Closed output channel '" << GetName() << "'." << Core::Log::Flush;
    }


}