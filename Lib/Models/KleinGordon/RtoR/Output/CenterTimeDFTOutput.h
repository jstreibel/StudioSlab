//
// Created by joao on 6/27/24.
//

#ifndef STUDIOSLAB_CENTERTIMEDFTOUTPUT_H
#define STUDIOSLAB_CENTERTIMEDFTOUTPUT_H

#include "Math/Numerics/Output/Plugs/Socket.h"

namespace Slab::Models::KGRtoR {

    struct TimeDFTOutputConfig {
        const Str &filename;
        Vector<Real> x_measure;
        Real t_start;
        Real t_end;
    };

    class CenterTimeDFTOutput : public Math::Socket {
        Str filename;
        Vector<RealVector> dataset;
        Vector<Real> x_measure;
        Real t_start, t_end;
        int step_start;

    protected:
        auto handleOutput(const Math::OutputPacket &packet) -> void override;

    public:
        auto computeNextRecStep(UInt currStep) -> size_t override;

    public:
        auto notifyIntegrationHasFinished(const Math::OutputPacket &theVeryLastOutputInformation) -> bool override;

    public:
        CenterTimeDFTOutput(const Math::NumericConfig &config, TimeDFTOutputConfig timeDFTSnapshotConfig);

    };
}

#endif //STUDIOSLAB_CENTERTIMEDFTOUTPUT_H