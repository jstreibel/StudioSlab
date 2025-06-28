
#include "OutputManager.h"
#include "Core/Tools/Log.h"


namespace Slab::Math {

    OutputManager::OutputManager(CountType max_steps) : maxSteps(max_steps) {}

    OutputManager::~OutputManager() = default; // No need to destroy output objects in vectors;

    void OutputManager::output(OutputPacket &infoVolatile, bool force) {
        const size_t steps = infoVolatile.getSteps();

        for (auto &out : outputs) {
            auto shouldOutput = out->shouldOutput(steps) || force;
            if (shouldOutput) {
                out->output(infoVolatile);
            }
        }

    }


    auto OutputManager::computeNStepsToNextOutput(UInt currStep) -> UInt {
        CountType nSteps = maxSteps;

        for (auto &socket: outputs) {
            const size_t nextRecStep = socket->computeNextRecStep(currStep);

            if (nextRecStep < nSteps) nSteps = nextRecStep;
        }

        return nSteps < currStep ? 1 : nSteps - currStep;
    }

    void OutputManager::addOutputChannel(Socket_ptr out) {
        outputs.push_back(out);

        Core::Log::Status() << "Output manager added "
                      << Core::Log::FGBlue    << out->getName()
                      << Core::Log::FGMagenta << " : "
                      << Core::Log::FGBlue    << out->getDescription()
                      << Core::Log::ResetFormatting << " output channel. Updates every "
                      << Core::Log::FGGreen << out->getnSteps()
                      << Core::Log::ResetFormatting << " sim steps."
                      << Core::Log::Flush;
    }

    void OutputManager::notifyIntegrationFinished(const OutputPacket &theVeryLastOutputInformation) {
        for (const auto& output: outputs) {
            if (!output->notifyIntegrationHasFinished(theVeryLastOutputInformation))
                Core::Log::Error() << "Error while finishing " << output->getName() << "..." << Core::Log::Flush;
        }


    }


}