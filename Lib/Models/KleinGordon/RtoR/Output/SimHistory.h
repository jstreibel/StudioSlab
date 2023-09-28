//
// Created by joao on 17/08/23.
//

#ifndef STUDIOSLAB_SIMHISTORY_H
#define STUDIOSLAB_SIMHISTORY_H


#include "Math/Numerics/SimConfig/SimulationConfig.h"
#include "Math/Numerics/Output/Plugs/Socket.h"
#include "Maps/R2toR/Model/R2toRDiscreteFunction.h"
#include "Maps/RtoR/Model/RtoRFunction.h"
#include "Maps/RtoR/Model/FunctionsCollection/Bypass.h"
#include "../KG-RtoREquationState.h"

class SimHistory : public Numerics::OutputSystem::Socket {
    R2toR::DiscreteFunction *data;
    RealVector timestamps;
    bool dataIsOnGPU;

    virtual auto filter(Real x, const RtoR::EquationState &input) -> Real;

protected:
    auto handleOutput(const OutputPacket &packet) -> void override;

public:
    SimHistory(const Core::Simulation::SimulationConfig &simConfig,
               Resolution N_x,
               Resolution N_t,
               Real xMin,
               Real L,
               const Str& name = "SimulationHistory");

    auto getData() const -> const R2toR::DiscreteFunction&;

};


#endif //STUDIOSLAB_SIMHISTORY_H
