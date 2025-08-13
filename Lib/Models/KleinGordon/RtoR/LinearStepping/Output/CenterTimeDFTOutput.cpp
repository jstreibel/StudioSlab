//
// Created by joao on 6/27/24.
//

#include "Utils/Zip.h"

#include "CenterTimeDFTOutput.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.h"

#include "Math/Function/RtoR/Operations/DiscreteFourierTransform.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "SnapshotOutput.h"

namespace Slab::Models::KGRtoR {

    Slab::Models::KGRtoR::CenterTimeDFTOutput::CenterTimeDFTOutput(
            DevFloat t_max, CountType max_steps, TimeDFTOutputConfig dftConfig)
    : Socket(Str("Single-location time-DFT"), 10,
             Str("t_start=")    + ToStr(Common::max(dftConfig.t_start, 0.0)) +
             Str(" ... t_end=") + ToStr(Common::min(dftConfig.t_end, t_max)))
    , filename(dftConfig.filename + ".time.dft.simsnap")
    , x_measure(dftConfig.x_measure)
    , dataset(dftConfig.x_measure.size())
    , t_start(Common::max(dftConfig.t_start, 0.0))
    , t_end  (Common::min(dftConfig.t_end, t_max))
    , step_start((int)(t_start/t_max * (DevFloat)max_steps))
    , step_end  ((int)(t_end  /t_max * (DevFloat)max_steps))
    {    }

    void Slab::Models::KGRtoR::CenterTimeDFTOutput::HandleOutput(const Slab::Math::OutputPacket &packet) {
        assert(x_measure.size() == dataset.size());

        fix curr_step = packet.GetSteps();
        if(curr_step < step_start || curr_step > step_end) return;

        auto funky = packet.GetNakedStateData<KGRtoR::EquationState>();

        IN phi = funky->getPhi();
        auto i=0;
        for(fix x : x_measure) {
            fix value = phi(x);
            dataset[i++].emplace_back(value);
        }
    }

    size_t CenterTimeDFTOutput::computeNextRecStep(UInt currStep) {
        if(currStep < step_start)
            return step_start;

        return Socket::computeNextRecStep(currStep);
    }

    bool CenterTimeDFTOutput::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) {
        Socket::notifyIntegrationHasFinished(theVeryLastOutputInformation);
        using DFT = Slab::Math::RtoR::DFT;

        Vector<TPointer<RtoR::NumericFunction>> maggies;
        for(auto &data : dataset) {
            auto slice = New<Math::RtoR::NumericFunction_CPU>(RealArray(data.data(), data.size()), t_start, t_end);
            auto result = DFT::Compute(*slice.get());
            maggies.emplace_back(DFT::Magnitudes(result));
        }

        auto average = maggies[0];
        for(auto i=1; i<maggies.size(); ++i ) {
            auto &maggie = *maggies[i];
            *average += maggie;
        }

        *average *= 1./(DevFloat)maggies.size();

        auto dω = average->getSpace().getMetaData().geth()[0];

        Vector<Pair<Str, Str>> py_entries;
        py_entries.emplace_back("dohm",           ToStr(dω)         );
        py_entries.emplace_back("ohm_mode_count", ToStr(average->N) );
        py_entries.emplace_back("t_start",        ToStr(t_start)    );
        py_entries.emplace_back("t_end",          ToStr(t_end)      );

        return SnapshotOutput::OutputNumericFunction(average->getSpace(), filename, py_entries);
    }

}