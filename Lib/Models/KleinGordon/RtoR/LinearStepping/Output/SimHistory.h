//
// Created by joao on 17/08/23.
//

#ifndef STUDIOSLAB_SIMHISTORY_H
#define STUDIOSLAB_SIMHISTORY_H

#include "Math/Numerics/OutputChannel.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.h"

namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    class SimHistory : public FOutputChannel {

    public:
        SimHistory(CountType MaxSteps, DevFloat tMax,
                   Resolution N_x,
                   Resolution N_t,
                   DevFloat xMin,
                   DevFloat L,
                   const Str &ChannelName = "SimulationHistory",
                   bool bManageData = false);

        [[nodiscard]] auto GetData() const -> TPointer<const R2toR::FNumericFunction>;
        [[nodiscard]] auto GetData()       -> TPointer<R2toR::FNumericFunction>;

    protected:
        const CountType MaxSteps;
        const DevFloat Max_t;
        const int N_t, N_x;

        auto HandleOutput(const OutputPacket &Packet) -> void override;

    private:
        R2toR::FNumericFunction_ptr Data;
        IntVector Timesteps;

        virtual auto Transfer(const OutputPacket &Packet, ValarrayWrapper <DevFloat> &DataOut) -> void;

    };


}

#endif //STUDIOSLAB_SIMHISTORY_H
