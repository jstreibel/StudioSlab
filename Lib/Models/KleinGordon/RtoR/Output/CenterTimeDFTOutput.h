//
// Created by joao on 6/27/24.
//

#ifndef STUDIOSLAB_CENTERTIMEDFTOUTPUT_H
#define STUDIOSLAB_CENTERTIMEDFTOUTPUT_H

#include "Math/Numerics/Output/Plugs/Socket.h"

namespace Slab::Models::KGRtoR {

    class CenterTimeDFTOutput : public Math::Socket {
        Str filename;
        RealVector data;

    protected:
        auto handleOutput(const Math::OutputPacket &packet) -> void override;

    public:
        auto notifyIntegrationHasFinished(const Math::OutputPacket &theVeryLastOutputInformation) -> bool override;

    public:
        CenterTimeDFTOutput(const Math::NumericConfig &config, const Str &filename);

    };
}

#endif //STUDIOSLAB_CENTERTIMEDFTOUTPUT_H
