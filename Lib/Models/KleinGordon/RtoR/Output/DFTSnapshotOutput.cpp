//
// Created by joao on 20/09/23.
//

#include "DFTSnapshotOutput.h"
#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"
#include "Maps/RtoR/Calc/FourierTransform.h"

#define nConfig (params)
#define NDFTModes (nConfig.getN()/2.+1.)
#define kMaxDFT (NDFTModes*2*M_PI/nConfig.getL())

namespace KleinGordon {
    namespace RtoR {
        ::RtoR::DiscreteFunction_CPU DFTSnapshotOutput::filterData(const OutputPacket &packet) {
            ::RtoR::DiscreteFunction_CPU dft((int)NDFTModes, 0.0, kMaxDFT);

            IN phi = packet.getEqStateData<::RtoR::EquationState>()->getPhi();
            fix pts = ::RtoR::FourierTransform::Compute(phi).getAbs()->getPoints();

            OUT dftSpace = dft.getSpace().getHostData(true);
            for(auto i=0; i<pts.size(); ++i) {
                fix &pt = pts[i];

                dftSpace[i] = pt.y;
            }

            return dft;
        }

        DFTSnapshotOutput::DFTSnapshotOutput(const NumericConfig &config, const Str &fileName)
        : SnapshotOutput(config, fileName + ".dft", "DFT snapshot output", "outputs the DFT of the last simulation instant") {}
    } // KleinGordon
} // RtoR