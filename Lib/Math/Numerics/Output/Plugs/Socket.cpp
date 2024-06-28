//
// Created by joao on 10/8/21.
//

#include "Socket.h"
#include "Core/Tools/Log.h"


namespace Slab::Math {

    Socket::Socket(const NumericConfig &params, Str name, int nStepsInterval, Str description)
            : intervalStepsBetweenOutputs(nStepsInterval), params(params), name(name), description(description),
              nextRecStep(1) {}


    auto Socket::getLastSimTime() -> Real { return lastPacket.getSimTime(); }

    auto Socket::getnSteps() const -> int { return intervalStepsBetweenOutputs; }

    auto Socket::setnSteps(int n) -> void {
        intervalStepsBetweenOutputs = n >= 1 ? n : 1;
    }

    auto Socket::computeNextRecStep(UInt currStep) -> size_t {
        if (nextRecStep > currStep)
            return nextRecStep;

        fix n = (Real) intervalStepsBetweenOutputs;
        fix m = (Real) (currStep + 1);

        nextRecStep = (int) (n * std::ceil(m / n));

        return nextRecStep;
    }

    auto Socket::shouldOutput(const Real t, const long unsigned timestep) -> bool {
        (void) t;

#if SHOULD_OUTPUT___MODE == INT_BASED
        return !(timestep % intervalStepsBetweenOutputs);
#elif SHOULD_OUTPUT___MODE == FLOAT_BASED
        return abs(T-lastT) > abs(recDT);
#endif

        throw "Boundary not implemented.";
    }

    void Socket::output(const OutputPacket &outData) {
        handleOutput(outData);
        lastPacket = outData;
    }

    auto Socket::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
        lastPacket = theVeryLastOutputInformation;
        return true;
    }

    auto Socket::getDescription() const -> Str { return description; }

    auto Socket::getName() const -> Str { return name; }

    Socket::~Socket() {
        Log::Info() << "Closed output channel '" << getName() << "'." << Log::Flush;
    }


}