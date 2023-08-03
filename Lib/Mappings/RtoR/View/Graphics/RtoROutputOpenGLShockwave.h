//
// Created by joao on 15/07/2021.
//

#ifndef V_SHAPE_RTOROUTPUTOPENGLSHOCKWAVE_H
#define V_SHAPE_RTOROUTPUTOPENGLSHOCKWAVE_H

#include "Models/KleinGordon/RtoR/RtoRMonitor.h"


namespace RtoR {
    class ShockwaveMonitor : public RtoR::Monitor {
        Real a0, E;

        ResizableDiscreteFunction surfaceEnergyHistory;
        ResizableDiscreteFunction innerEnergyHistory;

    public:
        explicit ShockwaveMonitor(const NumericParams &params, KGEnergy &hamiltonian, Real a0=1, Real E=1);
        void draw() override;

    };
}


#endif //V_SHAPE_RTOROUTPUTOPENGLSHOCKWAVE_H
