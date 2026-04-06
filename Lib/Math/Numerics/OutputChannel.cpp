//
// Created by joao on 10/8/21.
//

#include "OutputChannel.h"
#include "Core/Tools/Log.h"


namespace Slab::Math {

    FOutputChannel::FOutputChannel(Str name, int nStepsInterval, Str Description)
            : IntervalStepsBetweenOutputs(nStepsInterval >= 1 ? nStepsInterval : 1), Name(name),
              Description(Description), NextRecStep(0) {}

    auto FOutputChannel::Get_nSteps() const -> int { return IntervalStepsBetweenOutputs; }

    auto FOutputChannel::Set_nSteps(int n) -> void {
        IntervalStepsBetweenOutputs = n >= 1 ? n : 1;
        NextRecStep = 0;
    }

    auto FOutputChannel::ComputeNextRecStep(UInt currStep) -> size_t {
        if (NextRecStep > currStep)
            return NextRecStep;

        const auto interval = static_cast<size_t>(IntervalStepsBetweenOutputs);
        const auto nextStep = ((static_cast<size_t>(currStep) / interval) + 1) * interval;

        NextRecStep = nextStep;

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

    void FOutputChannel::Output(const FOutputPacket &outData) {
        HandleOutput(outData);
        LastPacket = outData;
    }

    auto FOutputChannel::NotifyIntegrationHasFinished(const FOutputPacket &TheVeryLastOutputInformation) -> bool {
        LastPacket = TheVeryLastOutputInformation;
        return true;
    }

    auto FOutputChannel::GetDescription() const -> Str { return Description; }

    auto FOutputChannel::GetName() const -> Str { return Name; }

    FOutputChannel::~FOutputChannel() {
        Core::FLog::Info() << "Closed output channel '" << GetName() << "'." << Core::FLog::Flush;
    }


}
