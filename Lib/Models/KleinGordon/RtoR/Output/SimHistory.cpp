//
// Created by joao on 17/08/23.
//

#include "SimHistory.h"

#include <utility>

#include "Maps/R2toR/Model/R2toRDiscreteFunctionCPU.h"

#if USE_CUDA
#include "Maps/R2toR/Model/R2toRDiscreteFunctionGPU.h"
#endif

#include "../KG-RtoREquationState.h"
#include "Core/Tools/Log.h"

#define tRes ( (N/L) * simConfig.numericConfig.gett() )

// const auto nₒᵤₜ = (Resolution)(Nₒᵤₜ*t/L);

SimHistory::SimHistory(const Core::Simulation::SimulationConfig &simConfig, Resolution N, Real xMin, Real L, const Str& name)
: Socket(simConfig.numericConfig, name, (int)(simConfig.numericConfig.getn()/ tRes),
         "A specific history tracker designed to watch the full sim history through the OpenGL (or whichever) monitors.")
, dataIsOnGPU(simConfig.dev==GPU)
{
    fix t = simConfig.numericConfig.gett();
    fix timeResolution = tRes;

    fix hx = L / (Real)N;
    fix ht = t / timeResolution;

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
        auto sizeMB = Real((Real)N*timeResolution*sizeof(Real))/(1024*1024.);

        Log::Critical() << name << " is about to allocate " << sizeMB
                        << "MB of data to store full " << N << 'x' << timeResolution << "x8 bytes simulation history." << Log::Flush;

        data = new R2toR::DiscreteFunction_CPU(N, (int)timeResolution, xMin, 0.0, hx, ht);

        Log::Success() << name << " allocated " << sizeMB << " of data." << Log::Flush;
    }
}

auto SimHistory::filter(Real x, const RtoR::EquationState &input) -> Real {
    IN f = input.getPhi();
    return f(x);
}

void SimHistory::handleOutput(const OutputPacket &packet) {
    if(packet.getSimTime() >= params.gett())
        return;

#if USE_CUDA
    if(dataIsOnGPU)
        throw "SimHistory::handleOutput with GPU data is not implemented";
    else
#endif
    {
        assert(packet.getEqStateData<RtoR::EquationState>() != nullptr);

        IN state = *packet.getEqStateData<RtoR::EquationState>();
        OUT to_ϕ = *data;

        fix xMin = to_ϕ.getDomain().xMin;
        fix xMax = to_ϕ.getDomain().xMax;
        fix dx = to_ϕ.getSpace().getMetaData().geth(0);

        fix t = packet.getSimTime();

        for(auto x=xMin; x<xMax; x+=dx)
            to_ϕ({x, t}) = filter(x, state);
    }

    timestamps.emplace_back(packet.getSimTime());
}

auto SimHistory::getData() const -> const R2toR::DiscreteFunction & {
    return *data;
}

