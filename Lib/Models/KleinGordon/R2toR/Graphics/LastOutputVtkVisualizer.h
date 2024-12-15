//
// Created by joao on 7/25/23.
//

#ifndef STUDIOSLAB_LASTOUTPUTVTKVISUALIZER_H
#define STUDIOSLAB_LASTOUTPUTVTKVISUALIZER_H

#include "Math/Numerics/Socket.h"

namespace Slab::Models::KGR2toR {

    class LastOutputVTKVisualizer : public Math::Socket {
        int outN;
    protected:
        auto handleOutput(const Math::OutputPacket &packet) -> void override {};

    public:
        explicit LastOutputVTKVisualizer(Count total_steps, int outN=256);

        auto notifyIntegrationHasFinished(const Math::OutputPacket &) -> bool override;

    };
} // R2toR


#endif //STUDIOSLAB_LASTOUTPUTVTKVISUALIZER_H
