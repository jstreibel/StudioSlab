//
// Created by joao on 17/08/23.
//

#ifndef STUDIOSLAB_SIMHISTORY_H
#define STUDIOSLAB_SIMHISTORY_H

#include "Math/Numerics/OutputChannel.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Bypass.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.h"


namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    class SimHistory : public FOutputChannel {
        R2toR::FNumericFunction_ptr data;
        IntVector timesteps;

        virtual auto Transfer(const OutputPacket &packet, ValarrayWrapper <DevFloat> &dataOut) -> void;

    protected:
        const CountType max_steps;
        const DevFloat max_t;
        const int N_t, N_x;

        auto HandleOutput(const OutputPacket &packet) -> void override;

    public:
        SimHistory(CountType max_steps, DevFloat t_max,
                   Resolution N_x,
                   Resolution N_t,
                   DevFloat xMin,
                   DevFloat L,
                   const Str &name = "SimulationHistory");

        auto getData() const -> const R2toR::FNumericFunction &;

    };


}

#endif //STUDIOSLAB_SIMHISTORY_H
