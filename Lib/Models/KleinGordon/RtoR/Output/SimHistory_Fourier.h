//
// Created by joao on 12/09/23.
//

#ifndef STUDIOSLAB_SIMHISTORY_FOURIER_H
#define STUDIOSLAB_SIMHISTORY_FOURIER_H

#include "SimHistory.h"

#include "Math/Function/Maps/RtoR/Calc/DiscreteFourierTransform.h"
#include "Math/Function/Maps/RtoC/FourierModes.h"
#include "Math/Function/Maps/RtoR/Model/FunctionsCollection/ComplexMagnitude.h"
#include "Math/Function/Maps/RtoR/Model/RtoRDiscreteFunctionCPU.h"

class SimHistory_DFT : public SimHistory {
public:
    struct DFTInstantResult {
        Real t;
        RtoR::DFTResult result;
    };

    typedef std::vector<DFTInstantResult> DFTDataHistory;
private:
    DFTDataHistory dftDataHistory;

    auto transfer(const OutputPacket &input, ValarrayWrapper<Real> &dataOut) -> void override;

public:
    explicit SimHistory_DFT(const Core::Simulation::SimulationConfig &simConfig, Resolution N_time);

    const DFTDataHistory &getDFTDataHistory() const;
};


#endif //STUDIOSLAB_SIMHISTORY_FOURIER_H
