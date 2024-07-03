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
    const Str &filename,
    Vector<Real> x_measure={0.0})
    : Socket(config, "Single-location time-DFT", Common::max(1, int(1./config.getr())), "outputs time DFT of center point of simulation")
    , filename(filename + ".time.dft.simsnap")
    , x_measure(x_measure)
    , dataset(x_measure.size()) {}

    void Slab::Models::KGRtoR::CenterTimeDFTOutput::handleOutput(const Slab::Math::OutputPacket &packet) {
        assert(x_measure.size() == dataset.size());

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
            auto slice = New<Math::RtoR::NumericFunction_CPU>(RealArray(data.data(), data.size()), 0.0, t);
            auto result = DFT::Compute(*slice.get());
            maggies.emplace_back(DFT::Magnitudes(result));
        }

        auto average = maggies[0];
        for(auto i=1; i<maggies.size(); ++i ) {
            auto &maggie = *maggies[i];
            *average += maggie;
        }

        *average *= 1./maggies.size();

        auto dω = average->getSpace().getMetaData().geth()[0];
        using Entry = Pair<Str,Str>;

        return SnapshotOutput::OutputNumericFunction(average, filename,
                                                     {Entry("dohm", ToStr(dω))});
    }

}