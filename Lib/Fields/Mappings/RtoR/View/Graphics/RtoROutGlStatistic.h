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


    };

}


#endif //STUDIOSLAB_RTOROUTGLSTATISTIC_H
