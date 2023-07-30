//
// Created by joao on 7/12/22.
//

#ifndef STUDIOSLAB_RTOROUTGLSTATISTIC_H
#define STUDIOSLAB_RTOROUTGLSTATISTIC_H

#include "RtoRMonitor.h"

namespace RtoR {

    class OutGLStatistic : public RtoR::Monitor {
    public:
        explicit OutGLStatistic(const NumericParams &);
        void draw() override;

    private:
        FuncArbResizable temperature1History;
        FuncArbResizable temperature2History;
        FuncArbResizable temperature3History;

        GraphRtoR *mTemperaturesGraph;
        GraphRtoR *mHistogramsGraph;
        GraphRtoR *mHistogramsGraph2;
        GraphRtoR *mHistogramsGraph3;
        GraphRtoR *mHistogramsGraph4;

    };

}


#endif //STUDIOSLAB_RTOROUTGLSTATISTIC_H
