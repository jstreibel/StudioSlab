//
// Created by joao on 24/06/2021.
//

#ifndef V_SHAPE_NOISEFUNCTION_H
#define V_SHAPE_NOISEFUNCTION_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {
    class NoiseFunction : public RtoR::Function {
        DevFloat min, max;

    public:
        NoiseFunction(DevFloat min, DevFloat max);

        DevFloat operator()(DevFloat x) const override;

    };

}


#endif //V_SHAPE_NOISEFUNCTION_H
