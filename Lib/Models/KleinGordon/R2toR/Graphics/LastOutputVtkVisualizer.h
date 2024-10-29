//
// Created by joao on 7/25/23.
//

#ifndef STUDIOSLAB_LASTOUTPUTVTKVISUALIZER_H
#define STUDIOSLAB_LASTOUTPUTVTKVISUALIZER_H

#include "Math/Numerics/Socket.h"

namespace Slab::Math::R2toR {

    class LastOutputVTKVisualizer : public Socket {
        int outN;
    protected:
        auto handleOutput(const OutputPacket &packet) -> void override {};

    public:
        LastOutputVTKVisualizer(const NumericConfig &params, int outN=256);

        auto notifyIntegrationHasFinished(const OutputPacket &) -> bool override;

    };
} // R2toR


#endif //STUDIOSLAB_LASTOUTPUTVTKVISUALIZER_H
