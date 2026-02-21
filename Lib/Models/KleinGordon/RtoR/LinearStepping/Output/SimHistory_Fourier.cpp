//
// Created by joao on 12/09/23.
//

#include "SimHistory_Fourier.h"

#define NDFTModes (N/2+1)
#define kMaxDFT (DevFloat(NDFTModes)*2*M_PI/L)


namespace Slab::Models::KGRtoR {

    FSimHistoryDFT::FSimHistoryDFT(CountType max_steps, DevFloat t_max, Resolution N, DevFloat L, Resolution N_time)
            : FSimHistory(max_steps, t_max, NDFTModes, N_time, 0, kMaxDFT, "SimulationHistory (k-space DFT)") {}

    auto FSimHistoryDFT::Transfer(const FOutputPacket &Input, FValarrayWrapper<DevFloat> &DataOut) -> void {
        IN stateIn = *Input.GetNakedStateData<KGRtoR::EquationState>();

        IN phi = dynamic_cast<RtoR::NumericFunction&>(stateIn.getPhi());

        auto fdftNewData = RtoR::FDFT::Compute(phi);

        fix dt = Max_t / (DevFloat)MaxSteps;
        auto result = FDFTInstantResult{Input.GetSteps()*dt, fdftNewData};
        DFTDataHistory.emplace_back(result);

        fix pts = fdftNewData.getMagnitudes()->GetPoints();
        assert(N_x == pts.size());

        for (auto i = 0; i < N_x; ++i)
            DataOut[i] = pts[i].y;
    }

    const FDFTDataHistory &FSimHistoryDFT::GetDFTDataHistory() const {
        return DFTDataHistory;
    }


}
