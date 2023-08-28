//
// Created by joao on 17/08/23.
//

#include "SimHistory.h"

#include "Mappings/R2toR/Model/R2toRDiscreteFunctionCPU.h"

#if USE_CUDA
#include "Mappings/R2toR/Model/R2toRDiscreteFunctionGPU.h"
#endif

#include "KG-RtoREquationState.h"
#include "Core/Tools/Log.h"


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
        auto sizeMB = Real(spaceResolution*timeResolution*sizeof(Real))/(1024*1024.);

        Log::Critical() << name << " is about to allocate " << sizeMB
                        << "MB of data to store full " << spaceResolution << 'x' << timeResolution << "x8 bytes simulation history." << Log::Flush;
        fieldData = new R2toR::DiscreteFunction_CPU(spaceResolution,
                                                    timeResolution,
                                                    params.getxMin(),
                                                    0.0, hp);

        Log::Success() << name << " allocated " << sizeMB << " of data." << Log::Flush;
    }
}

void SimHistory::handleOutput(const OutputPacket &packet) {
    assert(fieldData != nullptr);

    if(packet.getSimTime() >= params.gett())
        return;

    if(dataIsOnGPU)
        throw "SimHistory::handleOutput with GPU data is not implemented";
    else {
        IN from = packet.getEqStateData<RtoR::EquationState>()->getPhi();
        OUT to = *fieldData;

        fix xMin = to.getDomain().xMin;
        fix xMax = to.getDomain().xMax;
        fix dx = to.getSpace().geth();

        fix t = packet.getSimTime();

        for(auto x=xMin; x<xMax; x+=dx)
            to({x, t}) = from(x);
    }

    timestamps.emplace_back(packet.getSimTime());
}

auto SimHistory::getData() const -> const R2toR::DiscreteFunction & {
    return *fieldData;
}
