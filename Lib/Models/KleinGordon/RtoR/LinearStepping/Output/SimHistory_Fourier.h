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
        RtoR::DFTResult Result;
    };
    typedef Vector<DFTInstantResult> FDFTDataHistory;

    class SimHistory_DFT : public SimHistory {

    public:
        explicit SimHistory_DFT(CountType max_steps, DevFloat t_max, Resolution N, DevFloat L, Resolution N_time);

        const FDFTDataHistory &GetDFTDataHistory() const;

    private:
        FDFTDataHistory DFTDataHistory;

        auto Transfer(const FOutputPacket &Input, ValarrayWrapper<DevFloat> &DataOut) -> void override;
    };


}

#endif //STUDIOSLAB_SIMHISTORY_FOURIER_H
