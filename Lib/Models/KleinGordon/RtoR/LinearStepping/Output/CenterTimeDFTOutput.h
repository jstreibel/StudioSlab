//
// Created by joao on 6/27/24.
//

#ifndef STUDIOSLAB_CENTERTIMEDFTOUTPUT_H
#define STUDIOSLAB_CENTERTIMEDFTOUTPUT_H

#include "Math/Numerics/Socket.h"

namespace Slab::Models::KGRtoR {

    struct TimeDFTOutputConfig {
        const Str &filename;
        Vector<DevFloat> x_measure;
        DevFloat t_start;
        DevFloat t_end;
    };

    class CenterTimeDFTOutput final : public Math::Socket {
        Str filename;
        Vector<FRealVector> dataset;
        Vector<DevFloat> x_measure;
        DevFloat t_start, t_end;
        int step_start, step_end;

    protected:
        auto handleOutput(const Math::OutputPacket &packet) -> void override;

    public:
        auto computeNextRecStep(UInt currStep) -> size_t override;

    public:
        auto notifyIntegrationHasFinished(const Math::OutputPacket &theVeryLastOutputInformation) -> bool override;

    public:
        CenterTimeDFTOutput(DevFloat t_max, CountType max_steps, TimeDFTOutputConfig timeDFTSnapshotConfig);

    };
}

#endif //STUDIOSLAB_CENTERTIMEDFTOUTPUT_H
