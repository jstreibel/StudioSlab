//
// Created by joao on 17/08/23.
//

#include "SimHistory.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"

#define StepsInterval ((int) (max_steps / N_t))

namespace Slab::Models::KGRtoR {

    // const auto nₒᵤₜ = (Resolution)(Nₒᵤₜ*t/L);

    using Core::Log;

    SimHistory::SimHistory(Count max_steps, Real t_max, Resolution N_x, Resolution N_t,
                           Real xMin, Real L, const Str &name)
            : Socket(name, StepsInterval,
                     "A specific history tracker designed to watch the full sim history through visual monitors.")
            , max_steps(max_steps), max_t(t_max), N_x((int)N_x), N_t((int)N_t) {
        fix t = t_max;
        fix timeResolution =(Real) N_t;

        fix hx = L / (Real) N_x;
        fix ht = t / timeResolution;

        auto sizeMB = Real((Real) N_x * timeResolution * sizeof(Real)) / (1024 * 1024.);

        Log::Critical() << name << " is about to allocate " << sizeMB
                        << "MB of data to store full " << N_x << 'x' << (int) timeResolution + 1
                        << "x8 bytes simulation history." << Log::Flush;


        fix safeTimeResolution = timeResolution + 1;
        data = DataAlloc<R2toR::NumericFunction_CPU>("SimulatedHistory", N_x, (int) safeTimeResolution, xMin, 0.0, hx, ht);

        Log::Success() << name << " allocated " << sizeMB << " of data." << Log::Flush;
    }

    auto SimHistory::transfer(const OutputPacket &packet, ValarrayWrapper<Real> &dataOut) -> void {
        IN stateIn = *packet.GetNakedStateData<KGRtoR::EquationState>();

        IN f_in = dynamic_cast<RtoR::NumericFunction&>(stateIn.getPhi());
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
        if (packet.getSteps() > max_steps)
            return;

        IN stateIn = packet.GetNakedStateData<KGRtoR::EquationState>();


        assert(&stateIn != nullptr);

        fix M_in = (double) max_steps;
        fix M_out = (double) N_t;
        fix t_ratio = M_out / M_in;

        fix j_in = packet.getSteps();
        fix j_out = int(floor((Real)j_in * t_ratio));
        ValarrayWrapper<Real> instantData(&data->At(0, j_out), data->getN());

        transfer(packet, instantData);

        timesteps.emplace_back(packet.getSteps());
    }

    auto SimHistory::getData() const -> const R2toR::NumericFunction & {
        return *data;
    }


}