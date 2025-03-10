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
        Real t=0;
        Real len=1;
        Real k=2*M_PI/len;
    public:
        explicit SquareWave(Real wavelength);

        Real operator()(Real x) const override;

        void set_t(Real t);

        Str symbol() const override;

        Str generalName() const override;
    };

} // Modes

#endif //STUDIOSLAB_SQUAREWAVE_H
