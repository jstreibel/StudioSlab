//
// Created by joao on 15/12/24.
//

#include "KG2DSnapshotOutput.h"

#include <utility>

#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

#include "Models/KleinGordon/RtoR/LinearStepping/Output/SnapshotOutput.h"
#include "Models/KleinGordon/R2toR/EquationState.h"


namespace Slab::Models::KGR2toR {

    KG2DSnapshotOutput::KG2DSnapshotOutput(Str filename)
    : Math::FOutputChannel("KG ℝ²↦ℝ snapshot", -1, "Output field data from last simulation step."), filename(std::move(filename)) {

    }

    void KG2DSnapshotOutput::HandleOutput(const Math::FOutputPacket &) {

    }

    bool KG2DSnapshotOutput::NotifyIntegrationHasFinished(const Math::FOutputPacket &final_output) {
        if(!FOutputChannel::NotifyIntegrationHasFinished(final_output)) return false;

        using StateType = Slab::Math::R2toR::EquationState;
        IN stateData = final_output.GetNakedStateData<StateType>();

        if(stateData == nullptr) {
            Core::Log::Error() << "Failed casting StateData to Slab::Math::R2toR::EquationState." << Core::Log::Flush;

            return false;
        }

        auto &space = stateData->getPhi().getSpace();
        IN N = space.getMetaData().getN();
        KGRtoR::SnapshotOutput::OutputNumericFunction(space, filename, {{"outresX", ToStr(N[0])}, {"outresY", ToStr(N[1])}});

        return true;
    }

}