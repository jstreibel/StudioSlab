//
// Created by joao on 12/09/23.
//

#ifndef STUDIOSLAB_SIMHISTORY_FOURIER_H
#define STUDIOSLAB_SIMHISTORY_FOURIER_H

#include "SimHistory.h"

#include "Maps/RtoR/Calc/FourierTransform.h"
#include "Maps/RtoC/FourierModes.h"
#include "Maps/RtoR/Model/FunctionsCollection/ComplexMagnitude.h"
#include "Maps/RtoR/Model/RtoRDiscreteFunctionCPU.h"

class SimHistory_FourierTransform : public SimHistory {
    RtoC::FourierModes fourierModes;
    auto filter(Real x, const RtoR::EquationState &input) -> Real override;
protected:
    auto handleOutput(const OutputPacket &packet) -> void override;

public:
    SimHistory_FourierTransform(const Core::Simulation::SimulationConfig &simConfig, Resolution n, Real kMin, Real kMax);
};

class SimHistory_DFT : public SimHistory {
    RtoR::DiscreteFunction_CPU dft;

    auto filter(Real x, const RtoR::EquationState &input) -> Real override;

protected:
    auto handleOutput(const OutputPacket &packet) -> void override;

public:
    explicit SimHistory_DFT(const Core::Simulation::SimulationConfig &simConfig);
};


#endif //STUDIOSLAB_SIMHISTORY_FOURIER_H
