//
// Created by joao on 17/08/23.
//

#include "SimHistory.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"

#define StepsInterval ((int) (MaxSteps / N_t))

namespace Slab::Models::KGRtoR {

    // const auto nₒᵤₜ = (Resolution)(Nₒᵤₜ*t/L);

    using Core::Log;

    SimHistory::SimHistory(CountType MaxSteps, const DevFloat tMax, Resolution N_x, Resolution N_t,
    DevFloat xMin, DevFloat L, const Str &ChannelName, bool bManageData)
    : FOutputChannel(ChannelName, StepsInterval,
        "A specific history tracker designed to watch the full sim history through visual monitors.")
    , MaxSteps(MaxSteps), Max_t(tMax), N_x((int)N_x), N_t((int)N_t)
    {
        fix t = tMax;
        fix timeResolution =static_cast<DevFloat>(N_t);

        fix hx = L / (DevFloat) N_x;
        fix ht = t / timeResolution;

        auto sizeMB = DevFloat((DevFloat) N_x * timeResolution * sizeof(DevFloat)) / (1024 * 1024.);

        Log::Critical() << ChannelName << " is about to allocate " << sizeMB
                        << "MB of data to store full " << N_x << 'x' << (int) timeResolution + 1
                        << "x8 bytes simulation history." << Log::Flush;


        fix safeTimeResolution = timeResolution + 1;
        if (bManageData)
        {
            Data = DataAllocAndManage<R2toR::NumericFunction_CPU>(ChannelName, N_x, (int) safeTimeResolution, xMin, 0.0, hx, ht);
        }
        else
        {
            Data = DataAlloc<R2toR::NumericFunction_CPU>(ChannelName, N_x, (int) safeTimeResolution, xMin, 0.0, hx, ht);
        }

        Log::Success() << ChannelName << " allocated " << sizeMB << " of data." << Log::Flush;
    }

    auto SimHistory::Transfer(const OutputPacket &Packet, ValarrayWrapper<DevFloat> &DataOut) -> void {
        IN stateIn = *Packet.GetNakedStateData<EquationState>();

        IN f_in = dynamic_cast<RtoR::NumericFunction&>(stateIn.getPhi());
        IN in = f_in.getSpace().getHostData(true);

        fix N_in = f_in.N;
        fix N_out = N_x;
        fix x_ratio = (double) N_in / N_out;

        for (auto i_out = 0; i_out < N_out; ++i_out) {
            fix i_in = int(floor(i_out * x_ratio));

            DataOut[i_out] = in[i_in];
        }
    }

    void SimHistory::HandleOutput(const OutputPacket &Packet) {
        if (Packet.GetSteps() > MaxSteps)
            return;

        IN StateIn = Packet.GetNakedStateData<KGRtoR::EquationState>();


        assert(&StateIn != nullptr);

        fix M_in = (double) MaxSteps;
        fix M_out = (double) N_t;
        fix t_ratio = M_out / M_in;

        fix j_in = Packet.GetSteps();
        fix j_out = int(floor((DevFloat)j_in * t_ratio));
        ValarrayWrapper InstantData(&Data->At(0, j_out), Data->getN());

        Transfer(Packet, InstantData);

        Timesteps.emplace_back(Packet.GetSteps());
    }

    auto SimHistory::GetData() const -> TPointer<const R2toR::FNumericFunction>
    {
        return Data;
    }

    auto SimHistory::GetData() -> TPointer<R2toR::FNumericFunction>
    {
        return Data;
    }
}
