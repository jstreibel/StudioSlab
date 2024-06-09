//
// Created by joao on 3/16/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H


#include "Models/KleinGordon/RtoR/RtoRMonitor.h"

namespace Montecarlo {
    class Monitor : public RtoR::Monitor {
        public:
        explicit Monitor(const NumericConfig &params);
        void draw() override;

    private:
        RtoR::ResizableNumericFunction temperature1History;
        RtoR::ResizableNumericFunction temperature2History;
        RtoR::ResizableNumericFunction temperature3History;

        GraphRtoR *mTemperaturesGraph;
        GraphRtoR *mHistogramsGraph;
        GraphRtoR *mHistogramsGraph2;
        GraphRtoR *mHistogramsGraph3;
        GraphRtoR *mHistogramsGraph4;

    };
}


#endif //STUDIOSLAB_MONITOR_H
