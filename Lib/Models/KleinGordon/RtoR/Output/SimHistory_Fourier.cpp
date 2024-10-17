//
// Created by joao on 12/09/23.
//

#include "SimHistory_Fourier.h"

#define NDFTModes (N/2+1)
#define kMaxDFT (Real(NDFTModes)*2*M_PI/L)


namespace Slab::Models::KGRtoR {

    SimHistory_DFT::SimHistory_DFT(Count max_steps, Real t_max, Resolution N, Real L, Resolution N_time)
            : SimHistory(max_steps, t_max, NDFTModes, N_time, 0, kMaxDFT, "SimulationHistory (k-space DFT)") {}

    auto SimHistory_DFT::transfer(const OutputPacket &input, ValarrayWrapper<Real> &dataOut) -> void {
        IN stateIn = *input.GetNakedStateData<KGRtoR::EquationState>();

        IN phi = dynamic_cast<RtoR::NumericFunction&>(stateIn.getPhi());

        auto dftNewData = RtoR::DFT::Compute(phi);

        fix dt = max_t / (Real)max_steps;
        auto result = DFTInstantResult{input.getSteps()*dt, dftNewData};
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