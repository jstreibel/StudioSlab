//
// Created by joao on 7/12/22.
//

#ifndef STUDIOSLAB_RTOROUTGLSTATISTIC_H
#define STUDIOSLAB_RTOROUTGLSTATISTIC_H

#include "RtoROutputOpenGL.h"

namespace RtoR {

    class OutGLStatistic : public RtoR::OutputOpenGL {
    public:
        explicit OutGLStatistic( );
        void draw() override;

    private:
        FuncArbResizable temperature1History;
        FuncArbResizable temperature2History;
        FuncArbResizable temperature3History;

        GraphRtoR *mTemperaturesGraph;
        GraphRtoR *mHistogramsGraph;
        GraphRtoR *mHistogramsGraph2;

    };

}


#endif //STUDIOSLAB_RTOROUTGLSTATISTIC_H
