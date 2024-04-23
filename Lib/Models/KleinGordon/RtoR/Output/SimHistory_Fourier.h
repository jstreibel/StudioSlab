//
// Created by joao on 12/09/23.
//

#ifndef STUDIOSLAB_SIMHISTORY_FOURIER_H
#define STUDIOSLAB_SIMHISTORY_FOURIER_H

#include "SimHistory.h"

#include "Maps/RtoR/Calc/DiscreteFourierTransform.h"
#include "Maps/RtoC/FourierModes.h"
#include "Maps/RtoR/Model/FunctionsCollection/ComplexMagnitude.h"
#include "Maps/RtoR/Model/RtoRDiscreteFunctionCPU.h"




class SimHistory_FourierTransform : public SimHistory {
    RtoC::FourierModes fourierModes;
    auto filter(Real x, const RtoR::EquationState &input) -> Real override;
protected:
    auto handleOutput(const OutputPacket &packet) -> void override;

public:
    SimHistory_FourierTransform(const Core::Simulation::SimulationConfig &simConfig,
                                Resolution N_x, Resolution N_t, Real kMin, Real kMax);
};




class SimHistory_DFT : public SimHistory {
public:
    struct DFTInstantResult {
        Real t;
        RtoR::DFTResult result;
    };

    typedef std::vector<DFTInstantResult> DFTDataHistory;
private:
    RtoR::DiscreteFunction_CPU dft;
    DFTDataHistory dftDataHistory;

    auto filter(Real x, const RtoR::EquationState &input) -> Real override;

protected:
    auto handleOutput(const OutputPacket &packet) -> void override;

public:
    explicit SimHistory_DFT(const Core::Simulation::SimulationConfig &simConfig, Resolution N_time);

    const DFTDataHistory &getDFTDataHistory() const;
};


#endif //STUDIOSLAB_SIMHISTORY_FOURIER_H
