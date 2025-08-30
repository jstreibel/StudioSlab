//
// Created by joao on 8/30/25.
//

#ifndef STUDIOSLAB_DUMMYOUTPUT_H
#define STUDIOSLAB_DUMMYOUTPUT_H
#include "Math/Numerics/OutputChannel.h"

namespace Slab::Math
{
    class FDummyOutput : public FOutputChannel
    {
    protected:
        auto HandleOutput(const OutputPacket&) -> void override;

    public:
        FDummyOutput();


    };
}

#endif //STUDIOSLAB_DUMMYOUTPUT_H
