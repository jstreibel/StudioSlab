//
// Created by joao on 6/27/24.
//

#include "Utils/Zip.h"

#include "CenterTimeDFTOutput.h"
#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"

#include "Math/Function/RtoR/Operations/DiscreteFourierTransform.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "SnapshotOutput.h"

namespace Slab::Models::KGRtoR {

    Slab::Models::KGRtoR::CenterTimeDFTOutput::CenterTimeDFTOutput(
    const Slab::Math::NumericConfig &config,
    TimeDFTOutputConfig dftConfig)
    : Socket(config, "Single-location time-DFT", 10, "outputs time DFT of center point of simulation")
    , filename(dftConfig.filename + ".time.dft.simsnap")
    , x_measure(dftConfig.x_measure)
    , dataset(dftConfig.x_measure.size())
    , t_start(Common::max(dftConfig.t_start, 0.0))
    , t_end(dftConfig.t_end)
    , step_start((int)(t_start/params.gett() * params.getn()))
    {    }

    void Slab::Models::KGRtoR::CenterTimeDFTOutput::handleOutput(const Slab::Math::OutputPacket &packet) {
        assert(x_measure.size() == dataset.size());

        if(packet.getSteps() < step_start) return;

        auto funky = packet.GetNakedStateData<KGRtoR::EquationState>();

        IN phi = funky->getPhi();
        auto i=0;
        for(fix x : x_measure) {
            fix value = phi(x);
            dataset[i++].emplace_back(value);
        }
    }

    bool CenterTimeDFTOutput::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) {
        Socket::notifyIntegrationHasFinished(theVeryLastOutputInformation);
        using DFT = Slab::Math::RtoR::DFT;

        fix t = theVeryLastOutputInformation.getSimTime();

        Vector<Pointer<RtoR::NumericFunction>> maggies;
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

        *average *= 1./(Real)maggies.size();

        auto dω = average->getSpace().getMetaData().geth()[0];

        Vector<Pair<Str, Str>> py_entries;
        py_entries.emplace_back("dohm",           ToStr(dω)         );
        py_entries.emplace_back("ohm_mode_count", ToStr(average->N) );
        py_entries.emplace_back("t_start",        ToStr(t_start)    );
        py_entries.emplace_back("t_end",          ToStr(t_end)      );

        return SnapshotOutput::OutputNumericFunction(average, filename, py_entries);
    }

    size_t CenterTimeDFTOutput::computeNextRecStep(UInt currStep) {
        if(currStep < step_start)
            return step_start;

        return Socket::computeNextRecStep(currStep);
    }

}