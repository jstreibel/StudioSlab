
#include "OutputManager.h"
#include "Core/Tools/Log.h"


namespace Slab::Math {

    FOutputManager::FOutputManager(const CountType MaxSteps) : MaxSteps(MaxSteps) {}

    FOutputManager::~FOutputManager() = default; // No need to destroy output objects in vectors;

    void FOutputManager::Output(const FOutputPacket &InfoVolatile, const bool ForceOutput) const
    {
        const size_t Steps = InfoVolatile.GetSteps();

        for (auto &Out : Outputs) {
            if (Out->ShouldOutput(Steps) || ForceOutput) {
                Out->Output(InfoVolatile);
            }
        }

    }


    auto FOutputManager::ComputeNStepsToNextOutput(UInt CurrStep) -> UInt {
        CountType nSteps = MaxSteps;

        for (const auto &Channel: Outputs) {
            if (const size_t NextRecStep = Channel->ComputeNextRecStep(CurrStep); NextRecStep < nSteps) nSteps = NextRecStep;
        }

        return nSteps <= CurrStep ? 1 : nSteps - CurrStep;
    }

    void FOutputManager::AddOutputChannel(FOutputChannel_ptr out) {
        Outputs.push_back(out);

        Core::FLog::Status() << "Output manager added "
                      << Core::FLog::FGBlue    << out->GetName()
                      << Core::FLog::FGMagenta << " : "
                      << Core::FLog::FGBlue    << out->GetDescription()
                      << Core::FLog::ResetFormatting << " output channel. Updates every "
                      << Core::FLog::FGGreen << out->Get_nSteps()
                      << Core::FLog::ResetFormatting << " sim steps."
                      << Core::FLog::Flush;
    }

    void FOutputManager::SetMaxSteps(UInt MaxSteps)
    {
        this->MaxSteps = MaxSteps;
    }

    void FOutputManager::NotifyIntegrationFinished(const FOutputPacket &theVeryLastOutputInformation) const
    {
        for (const auto& output: Outputs) {
            if (!output->NotifyIntegrationHasFinished(theVeryLastOutputInformation))
                Core::FLog::Error() << "Error while finishing " << output->GetName() << "..." << Core::FLog::Flush;
        }
    }


}
