//
// Created by joao on 6/27/24.
//

#ifndef STUDIOSLAB_CENTERTIMEDFTOUTPUT_H
#define STUDIOSLAB_CENTERTIMEDFTOUTPUT_H

#include "Math/Numerics/OutputChannel.h"

namespace Slab::Models::KGRtoR {

    struct FTimeDFTOutputConfig {
        const Str &filename;
        Vector<DevFloat> x_measure;
        DevFloat t_start;
        DevFloat t_end;
    };

    class FCenterTimeDFTOutput final : public Math::FOutputChannel {
        Str filename;
        Vector<FRealVector> dataset;
        Vector<DevFloat> x_measure;
        DevFloat t_start, t_end;
        int step_start, step_end;

    protected:
        auto HandleOutput(const Math::FOutputPacket &packet) -> void override;

    public:
        auto ComputeNextRecStep(UInt currStep) -> size_t override;

    public:
        auto NotifyIntegrationHasFinished(const Math::FOutputPacket &theVeryLastOutputInformation) -> bool override;

    public:
        FCenterTimeDFTOutput(DevFloat t_max, CountType max_steps, FTimeDFTOutputConfig timeDFTSnapshotConfig);

    };

    using TimeDFTOutputConfig [[deprecated("Use FTimeDFTOutputConfig")]] = FTimeDFTOutputConfig;
    using CenterTimeDFTOutput [[deprecated("Use FCenterTimeDFTOutput")]] = FCenterTimeDFTOutput;
}

#endif //STUDIOSLAB_CENTERTIMEDFTOUTPUT_H
