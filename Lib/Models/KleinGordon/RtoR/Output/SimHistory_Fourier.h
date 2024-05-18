//
// Created by joao on 12/09/23.
//

#ifndef STUDIOSLAB_SIMHISTORY_FOURIER_H
#define STUDIOSLAB_SIMHISTORY_FOURIER_H

#include "SimHistory.h"

#include "Math/Function/RtoR/Calc/DiscreteFourierTransform.h"
#include "Math/Function/RtoC/FourierModes.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/ComplexMagnitude.h"
#include "Math/Function/RtoR/Model/RtoRDiscreteFunctionCPU.h"

struct DFTInstantResult {
    Real t;
    RtoR::DFTResult result;
};
typedef std::vector<DFTInstantResult> DFTDataHistory;

class SimHistory_DFT : public SimHistory {

private:
    DFTDataHistory dftDataHistory;

    auto transfer(const OutputPacket &input, ValarrayWrapper<Real> &dataOut) -> void override;

public:
    explicit SimHistory_DFT(const Core::Simulation::SimulationConfig &simConfig, Resolution N_time);

    const DFTDataHistory &getDFTDataHistory() const;
};


#endif //STUDIOSLAB_SIMHISTORY_FOURIER_H
