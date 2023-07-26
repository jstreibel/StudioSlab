//
// Created by joao on 7/25/23.
//

#ifndef STUDIOSLAB_LASTOUTPUTVTKVISUALIZER_H
#define STUDIOSLAB_LASTOUTPUTVTKVISUALIZER_H

#include "Phys/Numerics/Output/Plugs/Socket.h"

namespace R2toR {

    class LastOutputVTKVisualizer : public Numerics::OutputSystem::Socket {
        int outN;
    protected:
        auto _out(const OutputPacket &packet) -> void override {};

    public:
        LastOutputVTKVisualizer(const NumericParams &params, int outN=256);

        auto notifyIntegrationHasFinished(const OutputPacket &) -> bool override;

    };
} // R2toR


#endif //STUDIOSLAB_LASTOUTPUTVTKVISUALIZER_H
