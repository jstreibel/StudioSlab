//
// Created by joao on 17/08/23.
//

#include "SimHistory.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"

#define StepsInterval ((int) (simConfig.numericConfig.getn() / N_t))

namespace Slab::Models::KGRtoR {

// const auto nₒᵤₜ = (Resolution)(Nₒᵤₜ*t/L);

    SimHistory::SimHistory(const SimulationConfig &simConfig, Resolution N_x, Resolution N_t,
                           Real xMin, Real L, const Str &name)
            : Socket(name, StepsInterval,
                     "A specific history tracker designed to watch the full sim history through visual monitors.")
            , dataIsOnGPU(simConfig.dev == GPU)
            , max_steps(simConfig.numericConfig.getn()), max_t(simConfig.numericConfig.gett()), N_x((int)N_x), N_t((int)N_t) {
        fix t = simConfig.numericConfig.gett();
        fix timeResolution =(Real) N_t;

        fix hx = L / (Real) N_x;
        fix ht = t / timeResolution;

#if USE_CUDA
        if(dataIsOnGPU)
        {
            /* fieldData = new R2toR::NumericFunction_GPU(spaceResolution,
                                                        timeResolution,
                                                        params.getxMin(),
                                                        0.0, hp);
                                                        */

            NOT_IMPLEMENTED

        } else
#endif
        {
            auto sizeMB = Real((Real) N_x * timeResolution * sizeof(Real)) / (1024 * 1024.);

            Log::Critical() << name << " is about to allocate " << sizeMB
                            << "MB of data to store full " << N_x << 'x' << (int) timeResolution + 1
                            << "x8 bytes simulation history." << Log::Flush;


            fix safeTimeResolution = timeResolution + 1;
            data = New<R2toR::NumericFunction_CPU>(N_x, (int) safeTimeResolution, xMin, 0.0, hx, ht);

            Log::Success() << name << " allocated " << sizeMB << " of data." << Log::Flush;
        }
    }

    auto SimHistory::transfer(const OutputPacket &packet, ValarrayWrapper<Real> &dataOut) -> void {
        IN stateIn = *packet.GetNakedStateData<KGRtoR::EquationState>();

        IN f_in = static_cast<RtoR::NumericFunction&>(stateIn.getPhi());
        IN in = f_in.getSpace().getHostData(true);

        fix N_in = f_in.N;
        fix N_out = N_x;
        fix x_ratio = (double) N_in / N_out;

        for (auto i_out = 0; i_out < N_out; ++i_out) {
            fix i_in = int(floor(i_out * x_ratio));

            dataOut[i_out] = in[i_in];
        }
    }

    void SimHistory::handleOutput(const OutputPacket &packet) {
        if (packet.getSimTime() >= max_t)
            return;

        IN stateIn = packet.GetNakedStateData<KGRtoR::EquationState>();


        assert(&stateIn != nullptr);

#if USE_CUDA
        if(dataIsOnGPU)
            throw "SimHistory::handleOutput with GPU data is not implemented";
        else
#endif
        {
            fix M_in = (double) max_steps;
            fix M_out = (double) N_t;
            fix t_ratio = M_out / M_in;

            fix j_in = packet.getSteps();
            fix j_out = int(floor(j_in * t_ratio));
            ValarrayWrapper<Real> instantData(&data->At(0, j_out), data->getN());

            transfer(packet, instantData);
        }

        timestamps.emplace_back(packet.getSimTime());
    }

    auto SimHistory::getData() const -> const R2toR::NumericFunction & {
        return *data;
    }


}