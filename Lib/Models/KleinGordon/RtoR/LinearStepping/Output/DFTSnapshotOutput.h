//
// Created by joao on 20/09/23.
//

#ifndef STUDIOSLAB_DFTSNAPSHOTOUTPUT_H
#define STUDIOSLAB_DFTSNAPSHOTOUTPUT_H

#include "SnapshotOutput.h"


namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    class DFTSnapshotOutput : public SnapshotOutput {
    protected:
        const Count N_DFT_modes;
        const Real k_max;
        auto filterData(const OutputPacket &packet) -> RtoR::NumericFunction_CPU override;
    public:
        DFTSnapshotOutput(const Resolution N, const Real L, const Str &fileName);
    };
} // KleinGordon::RtoR

#endif //STUDIOSLAB_DFTSNAPSHOTOUTPUT_H
