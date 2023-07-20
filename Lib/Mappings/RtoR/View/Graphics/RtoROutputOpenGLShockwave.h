//
// Created by joao on 15/07/2021.
//

#ifndef V_SHAPE_RTOROUTPUTOPENGLSHOCKWAVE_H
#define V_SHAPE_RTOROUTPUTOPENGLSHOCKWAVE_H

#include "RtoRMonitor.h"


namespace RtoR {
    class OutputOpenGLShockwave : public RtoR::Monitor {
        Real a0, E;

        FuncArbResizable surfaceEnergyHistory;
        FuncArbResizable innerEnergyHistory;

    public:
        explicit OutputOpenGLShockwave(const NumericParams &params, Real a0=1, Real E=1);
        void draw() override;

    };
}


#endif //V_SHAPE_RTOROUTPUTOPENGLSHOCKWAVE_H
