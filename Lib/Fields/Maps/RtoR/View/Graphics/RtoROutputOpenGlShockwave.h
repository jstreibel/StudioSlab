//
// Created by joao on 15/07/2021.
//

#ifndef V_SHAPE_RTOROUTPUTOPENGLSHOCKWAVE_H
#define V_SHAPE_RTOROUTPUTOPENGLSHOCKWAVE_H

#include "RtoROutputOpenGL.h"


namespace RtoR {
    class OutputOpenGLShockwave : public RtoR::OutputOpenGL {
        double a0, E;

        FuncArbResizable surfaceEnergyHistory;
        FuncArbResizable innerEnergyHistory;

    public:
        explicit OutputOpenGLShockwave(double a0=1, double E=1);
        void draw() override;

    };
}


#endif //V_SHAPE_RTOROUTPUTOPENGLSHOCKWAVE_H
