//
// Created by joao on 17/08/23.
//

#ifndef STUDIOSLAB_SIMHISTORY_H
#define STUDIOSLAB_SIMHISTORY_H


#include "Math/Numerics/SimConfig/SimulationConfig.h"
#include "Math/Numerics/Output/Plugs/Socket.h"
#include "Mappings/R2toR/Model/R2toRDiscreteFunction.h"

class SimHistory : public Numerics::OutputSystem::Socket {
    R2toR::DiscreteFunction *fieldData;
    RealVector timestamps;
    bool dataIsOnGPU;

    auto handleOutput(const OutputPacket &packet) -> void override;
public:
    SimHistory(const Core::Simulation::SimulationConfig &simConfig,
                     PosInt                              timeResolution,
                     PosInt                              spaceResolution);

    auto getData() const -> const R2toR::DiscreteFunction&;
};


#endif //STUDIOSLAB_SIMHISTORY_H
