//
// Created by joao on 29/03/24.
//

#include "SquareWave.h"

namespace Modes {
    SquareWave::SquareWave(Real wavelength)
    : FunctionT(nullptr, false)
    , len(wavelength)
    , k(2*M_PI/len){}

    Real SquareWave::operator()(Real x) const {
        auto parity = int(floorf64(2*(x+t)*len));
        return - (2*(parity%2) - 1);

        // auto val = 0.0;
        // for (int n=1; n<=40; n+=2)
        //     val += (1./n)*sin(n*k*(x-t));
        // return 4*M_1_PI*val;
    }

    Str SquareWave::symbol() const {
        return "Sqₜ(x)";
    }

    Str SquareWave::myName() const {
        return "Square wave";
    }

    void SquareWave::set_t(Real t) {
        this->t = t;
    }


} // Modes