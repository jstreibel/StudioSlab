//
// Created by joao on 17/08/23.
//

#include "SimHistory.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"

#define StepsInterval ((int) (max_steps / N_t))

namespace Slab::Models::KGRtoR {

    // const auto nₒᵤₜ = (Resolution)(Nₒᵤₜ*t/L);

    using Core::Log;

    SimHistory::SimHistory(CountType max_steps, DevFloat t_max, Resolution N_x, Resolution N_t,
                           DevFloat xMin, DevFloat L, const Str &name)
            : FOutputChannel(name, StepsInterval,
                     "A specific history tracker designed to watch the full sim history through visual monitors.")
            , max_steps(max_steps), max_t(t_max), N_x((int)N_x), N_t((int)N_t) {
        fix t = t_max;
        fix timeResolution =(DevFloat) N_t;

        fix hx = L / (DevFloat) N_x;
        fix ht = t / timeResolution;

        auto sizeMB = DevFloat((DevFloat) N_x * timeResolution * sizeof(DevFloat)) / (1024 * 1024.);

        Log::Critical() << name << " is about to allocate " << sizeMB
                        << "MB of data to store full " << N_x << 'x' << (int) timeResolution + 1
                        << "x8 bytes simulation history." << Log::Flush;


        fix safeTimeResolution = timeResolution + 1;
        data = DataAlloc<R2toR::NumericFunction_CPU>("SimulatedHistory", N_x, (int) safeTimeResolution, xMin, 0.0, hx, ht);

        Log::Success() << name << " allocated " << sizeMB << " of data." << Log::Flush;
    }

    auto SimHistory::Transfer(const OutputPacket &packet, ValarrayWrapper<DevFloat> &dataOut) -> void {
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

    void SimHistory::HandleOutput(const OutputPacket &packet) {
        if (packet.GetSteps() > max_steps)
            return;

        IN stateIn = packet.GetNakedStateData<KGRtoR::EquationState>();


        assert(&stateIn != nullptr);

        fix M_in = (double) max_steps;
        fix M_out = (double) N_t;
        fix t_ratio = M_out / M_in;

        fix j_in = packet.GetSteps();
        fix j_out = int(floor((DevFloat)j_in * t_ratio));
        ValarrayWrapper<DevFloat> instantData(&data->At(0, j_out), data->getN());

        Transfer(packet, instantData);

        timesteps.emplace_back(packet.GetSteps());
    }

    auto SimHistory::getData() const -> const R2toR::FNumericFunction & {
        return *data;
    }


}