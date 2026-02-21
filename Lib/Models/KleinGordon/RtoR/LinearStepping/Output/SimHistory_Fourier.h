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

    struct FDFTInstantResult {
        DevFloat t{};
        RtoR::FDFTResult Result;
    };
    using FDFTDataHistory = Vector<FDFTInstantResult>;

    class FSimHistoryDFT : public FSimHistory {

    public:
        explicit FSimHistoryDFT(CountType max_steps, DevFloat t_max, Resolution N, DevFloat L, Resolution N_time);

        const FDFTDataHistory &GetDFTDataHistory() const;

    private:
        FDFTDataHistory DFTDataHistory;

        auto Transfer(const FOutputPacket &Input, FValarrayWrapper<DevFloat> &DataOut) -> void override;
    };

    using DFTInstantResult [[deprecated("Use FDFTInstantResult")]] = FDFTInstantResult;
    using SimHistory_DFT [[deprecated("Use FSimHistoryDFT")]] = FSimHistoryDFT;

}

#endif //STUDIOSLAB_SIMHISTORY_FOURIER_H
