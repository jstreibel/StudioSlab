//
// Created by joao on 29/03/24.
//

#ifndef STUDIOSLAB_SQUAREWAVE_H
#define STUDIOSLAB_SQUAREWAVE_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Modes {

    using namespace Slab;
    using namespace Slab::Math;

    class SquareWave : public RtoR::Function {
        DevFloat t=0;
        DevFloat len=1;
        DevFloat k=2*M_PI/len;
    public:
        explicit SquareWave(DevFloat wavelength);

        DevFloat operator()(DevFloat x) const override;

        void set_t(DevFloat t);

        Str Symbol() const override;

        Str generalName() const override;
    };

} // Modes

#endif //STUDIOSLAB_SQUAREWAVE_H
