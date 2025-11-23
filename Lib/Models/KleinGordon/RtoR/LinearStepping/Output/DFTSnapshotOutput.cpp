//
// Created by joao on 20/09/23.
//

#include "DFTSnapshotOutput.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.h"
#include "Math/Function/RtoR/Operations/DiscreteFourierTransform.h"

#define NDFTModes (N/2.+1.)
#define kMaxDFT (NDFTModes*2*M_PI/L)

namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    DFTSnapshotOutput::DFTSnapshotOutput(const Resolution N, const DevFloat L, const Str &fileName)
    : SnapshotOutput(fileName + ".dft", "DFT snapshot output", "outputs the DFT of the last simulation instant")
    , N_DFT_modes(NDFTModes)
    , k_max(kMaxDFT)
    {   }


    RtoR::NumericFunction_CPU DFTSnapshotOutput::filterData(const FOutputPacket &packet) {
        RtoR::NumericFunction_CPU dft(static_cast<int>(N_DFT_modes), 0.0, k_max);

        IN kgState = *packet.GetNakedStateData<KGRtoR::EquationState>();

        IN phi = dynamic_cast<RtoR::NumericFunction&>(kgState.getPhi());
        fix pts = RtoR::DFT::Compute(phi).getMagnitudes()->GetPoints();

        OUT dftSpace = dft.getSpace().getHostData(true);
        for(auto i=0; i<pts.size(); ++i) {
            fix &pt = pts[i];

            dftSpace[i] = pt.y;
        }

        return dft;
    }

} // KleinGordon::RtoR