//
// Created by joao on 20/09/23.
//

#ifndef STUDIOSLAB_DFTSNAPSHOTOUTPUT_H
#define STUDIOSLAB_DFTSNAPSHOTOUTPUT_H

#include "SnapshotOutput.h"


namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    class FDFTSnapshotOutput final : public FSnapshotOutput {
    protected:
        const CountType N_DFT_modes;
        const DevFloat k_max;
        auto filterData(const FOutputPacket &packet) -> RtoR::NumericFunction_CPU override;
    public:
        FDFTSnapshotOutput(Resolution N, DevFloat L, const Str &fileName);
    };

    using DFTSnapshotOutput [[deprecated("Use FDFTSnapshotOutput")]] = FDFTSnapshotOutput;
} // KleinGordon::RtoR

#endif //STUDIOSLAB_DFTSNAPSHOTOUTPUT_H
