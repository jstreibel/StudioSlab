
#include "OutputManager.h"
#include "Core/Tools/Log.h"


namespace Slab::Math {

    OutputManager::OutputManager(const NumericConfig &params) : params(params), maxSteps(params.getn()) {}

    OutputManager::~OutputManager() = default; // No need to destroy output objects in vectors;

    void OutputManager::output(OutputPacket &infoVolatile, bool force) {
        const Real t = infoVolatile.getSimTime();
        const size_t steps = infoVolatile.getSteps();

        for (auto &out : outputs) {
            auto shouldOutput = out->shouldOutput(t, steps) || force;
            if (shouldOutput) {
                out->output(infoVolatile);
            }
        }

    }


    auto OutputManager::computeNStepsToNextOutput(UInt currStep) -> UInt {
        Count nSteps = maxSteps;

        for (auto &socket: outputs) {
            const size_t nextRecStep = socket->computeNextRecStep(currStep);

            if (nextRecStep < nSteps) nSteps = nextRecStep;
        }

        return nSteps < currStep ? 1 : nSteps - currStep;
    }

    void OutputManager::addOutputChannel(Socket_ptr out) {
        outputs.push_back(out);

        Log::Status() << "Output manager added '" << out->getName() << "' output channel. Updates "
                      << "every " << out->getnSteps() << " sim steps." << Log::Flush;
    }

    void OutputManager::notifyIntegrationFinished(const OutputPacket &theVeryLastOutputInformation) {
        Log::Success() << "Simulation finished. Total time steps: "
                       << theVeryLastOutputInformation.getSteps() << Log::Flush;

        for (auto output: outputs) {
            if (!output->notifyIntegrationHasFinished(theVeryLastOutputInformation))
                Log::Error() << "Error while finishing " << output->getName() << "..." << Log::Flush;

            Log::Info() << "Finished output channel '" << output->getName() << "'." << Log::Flush;
        }


    }


}