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
        auto filterData(const OutputPacket &packet) -> RtoR::NumericFunction_CPU override;
    public:
        DFTSnapshotOutput(const NumericConfig &config, const Str &fileName);
    };
} // KleinGordon::RtoR

#endif //STUDIOSLAB_DFTSNAPSHOTOUTPUT_H
