//
// Created by joao on 17/08/23.
//

#include "SimHistory.h"

#include "Mappings/R2toR/Model/R2toRDiscreteFunctionCPU.h"

#if USE_CUDA
#include "Mappings/R2toR/Model/R2toRDiscreteFunctionGPU.h"
#endif

#include "KG-RtoREquationState.h"


SimHistory::SimHistory(const Base::Simulation::SimulationConfig &simConfig,
                       PosInt timeResolution, PosInt spaceResolution)
: Socket(simConfig.numericConfig, "SimHistory", (int)(simConfig.numericConfig.getn()/ timeResolution), "A specific history tracker designed to watch the full sim history through the OpenGL (or whichever) monitors")
, dataIsOnGPU(simConfig.dev==GPU)
{
    fix hp = params.getL() / (Real)spaceResolution;

    #if USE_CUDA
    if(dataIsOnGPU)
    {
        fieldData = new R2toR::DiscreteFunction_GPU(spaceResolution,
                                                    timeResolution,
                                                    params.getxMin(),
                                                    0.0, hp);
    } else
    #endif
    {
        fieldData = new R2toR::DiscreteFunction_CPU(spaceResolution,
                                                    timeResolution,
                                                    params.getxMin(),
                                                    0.0, hp);
    }
}

void SimHistory::handleOutput(const OutputPacket &packet) {
    assert(fieldData != nullptr);

    if(dataIsOnGPU)
        throw "SimHistory::handleOutput with GPU data is not implemented";
    else {
        IN from = packet.getEqStateData<RtoR::EquationState>()->getPhi();
        OUT to = *fieldData;

        fix xMin = to.getDomain().xMin;
        fix xMax = to.getDomain().xMax;
        fix dx = to.getSpace().geth();

        fix t = packet.getSimTime();

        for(auto x=xMin+.5*dx; x<xMax; x+=dx)
            to({x, t}) = from(x);
    }

    timestamps.emplace_back(packet.getSimTime());
}

auto SimHistory::getData() const -> const R2toR::DiscreteFunction & {
    return *fieldData;
}
