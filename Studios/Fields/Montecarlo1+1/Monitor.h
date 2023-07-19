//
// Created by joao on 3/16/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H


#include "Mappings/RtoR/View/Graphics/RtoRMonitor.h"

namespace Montecarlo {
    class Monitor : public RtoR::Monitor {
        public:
        explicit Monitor( );
        void draw() override;

    private:
        RtoR::FuncArbResizable temperature1History;
        RtoR::FuncArbResizable temperature2History;
        RtoR::FuncArbResizable temperature3History;

        GraphRtoR *mTemperaturesGraph;
        GraphRtoR *mHistogramsGraph;
        GraphRtoR *mHistogramsGraph2;
        GraphRtoR *mHistogramsGraph3;
        GraphRtoR *mHistogramsGraph4;

    };
}


#endif //STUDIOSLAB_MONITOR_H
