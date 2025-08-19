//
// Created by joao on 12/09/23.
//

#include "SimHistory_Fourier.h"

#define NDFTModes (N/2+1)
#define kMaxDFT (DevFloat(NDFTModes)*2*M_PI/L)


namespace Slab::Models::KGRtoR {

    SimHistory_DFT::SimHistory_DFT(CountType max_steps, DevFloat t_max, Resolution N, DevFloat L, Resolution N_time)
            : SimHistory(max_steps, t_max, NDFTModes, N_time, 0, kMaxDFT, "SimulationHistory (k-space DFT)") {}

    auto SimHistory_DFT::Transfer(const OutputPacket &Input, ValarrayWrapper<DevFloat> &DataOut) -> void {
        IN stateIn = *Input.GetNakedStateData<KGRtoR::EquationState>();

        IN phi = dynamic_cast<RtoR::NumericFunction&>(stateIn.getPhi());

        auto DftNewData = RtoR::DFT::Compute(phi);

        fix dt = Max_t / (DevFloat)MaxSteps;
        auto result = DFTInstantResult{Input.GetSteps()*dt, DftNewData};
        DFTDataHistory.emplace_back(result);

        fix pts = DftNewData.getMagnitudes()->getPoints();
        assert(N_x == pts.size());

        for (auto i = 0; i < N_x; ++i)
            DataOut[i] = pts[i].y;
    }

    const FDFTDataHistory &SimHistory_DFT::GetDFTDataHistory() const {
        return DFTDataHistory;
    }


}