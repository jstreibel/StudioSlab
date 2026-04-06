//
// Created by joao on 7/25/23.
//

#ifndef STUDIOSLAB_LASTOUTPUTVTKVISUALIZER_H
#define STUDIOSLAB_LASTOUTPUTVTKVISUALIZER_H

#include "Math/Numerics/Socket.h"

namespace Slab::Models::KGR2toR {

    class FLastOutputVTKVisualizer : public Math::Socket {
        int outN;
    protected:
        auto HandleOutput(const Math::OutputPacket &packet) -> void override {};

    public:
        explicit FLastOutputVTKVisualizer(Count total_steps, int outN = 256);
        ~FLastOutputVTKVisualizer() override;

        auto notifyIntegrationHasFinished(const Math::OutputPacket &theVeryLastOutputInformation) -> bool override;

    };

    using LastOutputVTKVisualizer [[deprecated("Use FLastOutputVTKVisualizer")]] = FLastOutputVTKVisualizer;
} // R2toR


#endif //STUDIOSLAB_LASTOUTPUTVTKVISUALIZER_H
