//
// Created by joao on 12/09/23.
//

#include "SimHistory_Fourier.h"

#define nConfig (simConfig.numericConfig)
#define NDFTModes (nConfig.getN()/2+1)
#define kMaxDFT (NDFTModes*2*M_PI/nConfig.getL())


namespace Slab::Models::KGRtoR {

    SimHistory_DFT::SimHistory_DFT(const SimulationConfig &simConfig, Resolution N_time)
            : SimHistory(simConfig, NDFTModes, N_time, 0, kMaxDFT) {}

    auto SimHistory_DFT::transfer(const OutputPacket &input, ValarrayWrapper<Real> &dataOut) -> void {
        IN stateIn = *input.getEqStateData<EquationState>();

        IN phi = stateIn.getPhi();
        IN dataIn = phi.getSpace().getHostData(true);

        auto dftNewData = RtoR::DFT::Compute(phi);

        auto result = DFTInstantResult{input.getSimTime(), dftNewData};
        dftDataHistory.emplace_back(result);

        fix pts = dftNewData.getMagnitudes()->getPoints();
        assert(N_x == pts.size());

        for (auto i = 0; i < N_x; ++i)
            dataOut[i] = pts[i].y;
    }

    const DFTDataHistory &SimHistory_DFT::getDFTDataHistory() const {
        return dftDataHistory;
    }


}