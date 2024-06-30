//
// Created by joao on 6/27/24.
//

#include "CenterTimeDFTOutput.h"
#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"

#include "Math/Function/RtoR/Operations/DiscreteFourierTransform.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "SnapshotOutput.h"

namespace Slab::Models::KGRtoR {

    Slab::Models::KGRtoR::CenterTimeDFTOutput::CenterTimeDFTOutput(const Slab::Math::NumericConfig &config, const Str &filename)
    : Socket(config, "Single-location time-DFT", Common::max(1, int(1./config.getr())), "outputs time DFT of center point of simulation")
    , filename(filename + ".time.dft.simsnap"){}

    void Slab::Models::KGRtoR::CenterTimeDFTOutput::handleOutput(const Slab::Math::OutputPacket &packet) {
        auto funky = packet.GetNakedStateData<KGRtoR::EquationState>();

        auto xCenter = this->params.getxMin() + .5*this->params.getL();

        IN phi = funky->getPhi();
        fix value = phi(xCenter);
        data.emplace_back(value);
    }

    bool CenterTimeDFTOutput::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) {
        Socket::notifyIntegrationHasFinished(theVeryLastOutputInformation);

        auto t = theVeryLastOutputInformation.getSimTime();
        auto slice = New<Math::RtoR::NumericFunction_CPU>(RealArray(data.data(), data.size()), 0.0, t);

        using DFT = Slab::Math::RtoR::DFT;
        auto result = DFT::Compute(*slice.get());
        auto maggies = DFT::Magnitudes(result);

        auto dω = maggies->getSpace().getMetaData().geth()[0];
        using Entry = Pair<Str,Str>;

        return SnapshotOutput::OutputNumericFunction(maggies, filename,
                                                     {Entry("dohm", ToStr(dω))});
    }

}