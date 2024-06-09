//
// Created by joao on 20/09/23.
//

#include "DFTSnapshotOutput.h"
#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"
#include "Math/Function/RtoR/Operations/DiscreteFourierTransform.h"

#define nConfig (params)
#define NDFTModes (nConfig.getN()/2.+1.)
#define kMaxDFT (NDFTModes*2*M_PI/nConfig.getL())

namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    DFTSnapshotOutput::DFTSnapshotOutput(const NumericConfig &config, const Str &fileName)
    : SnapshotOutput(config, fileName + ".dft", "DFT snapshot output", "outputs the DFT of the last simulation instant")
    {   }


    RtoR::NumericFunction_CPU DFTSnapshotOutput::filterData(const OutputPacket &packet) {
        RtoR::NumericFunction_CPU dft((int)NDFTModes, 0.0, kMaxDFT);

        IN kgState = *packet.GetNakedStateData<KGRtoR::EquationState>();

        IN phi = static_cast<RtoR::NumericFunction&>(kgState.getPhi());
        fix pts = RtoR::DFT::Compute(phi).getMagnitudes()->getPoints();

        OUT dftSpace = dft.getSpace().getHostData(true);
        for(auto i=0; i<pts.size(); ++i) {
            fix &pt = pts[i];

            dftSpace[i] = pt.y;
        }

        return dft;
    }

} // KleinGordon::RtoR