//
// Created by joao on 29/03/24.
//

#include "SquareWave.h"

namespace Modes {
    FSquareWave::FSquareWave(DevFloat wavelength)
    : FunctionT(nullptr, false)
    , len(wavelength)
    , k(2*M_PI/len){}

    DevFloat FSquareWave::operator()(DevFloat x) const {
        auto parity = int(floorf64(2*(x+t)*len));
        return - (2*(parity%2) - 1);

        // auto val = 0.0;
        // for (int n=1; n<=40; n+=2)
        //     val += (1./n)*sin(n*k*(x-t));
        // return 4*M_1_PI*val;
    }

    Str FSquareWave::Symbol() const {
        return "Sqₜ(x)";
    }

    Str FSquareWave::generalName() const {
        return "Square wave";
    }

    void FSquareWave::set_t(DevFloat t) {
        this->t = t;
    }


} // Modes
