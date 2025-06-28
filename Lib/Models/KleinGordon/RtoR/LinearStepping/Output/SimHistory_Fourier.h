//
// Created by joao on 12/09/23.
//

#ifndef STUDIOSLAB_SIMHISTORY_FOURIER_H
#define STUDIOSLAB_SIMHISTORY_FOURIER_H

#include "SimHistory.h"

#include "Math/Function/RtoR/Operations/DiscreteFourierTransform.h"
#include "Math/Function/RtoC/FourierModes.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/ComplexMagnitude.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"


namespace Slab::Models::KGRtoR {

    struct DFTInstantResult {
        DevFloat t{};
        RtoR::DFTResult result;
    };
    typedef Vector<DFTInstantResult> DFTDataHistory;

    class SimHistory_DFT : public SimHistory {

    private:
        DFTDataHistory dftDataHistory;

        auto transfer(const OutputPacket &input, ValarrayWrapper<DevFloat> &dataOut) -> void override;

    public:
        explicit SimHistory_DFT(CountType max_steps, DevFloat t_max, Resolution N, DevFloat L, Resolution N_time);

        const DFTDataHistory &getDFTDataHistory() const;
    };


}

#endif //STUDIOSLAB_SIMHISTORY_FOURIER_H
