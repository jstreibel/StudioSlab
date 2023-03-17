//
// Created by joao on 3/16/23.
//

#ifndef STUDIOSLAB_RTOROUTGLMONTECARLO_H
#define STUDIOSLAB_RTOROUTGLMONTECARLO_H


#include "Fields/Mappings/RtoR/View/Graphics/RtoROutputOpenGL.h"

namespace Montecarlo {
    class OutGL : public RtoR::OutputOpenGL {
        public:
        explicit OutGL( );
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


#endif //STUDIOSLAB_RTOROUTGLMONTECARLO_H
