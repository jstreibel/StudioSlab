//
// Created by joao on 11/09/23.
//

#include "FourierModes.h"

#include <utility>

namespace Slab::Math::RtoC {
    FourierModes::FourierModes(RtoR::Function_ptr baseFunc, DevFloat xMin, DevFloat L, CountType samples)
    : baseFunc(std::move(baseFunc)), xMin(xMin), L(L), samples(samples) {

    }

    Complex FourierModes::operator()(DevFloat k) const {
        if(baseFunc == nullptr) return {};

        fix dx = L/(DevFloat)samples;

        auto &f = *baseFunc;

        fix ik = Complex(0,k);
        auto aₖ = Complex();
        for(auto n=0; n<samples; ++n) {
           fix x = xMin + (DevFloat)n*dx;

           aₖ += f(x)*std::exp(-ik*x);
        }

        return aₖ*dx;
    }

    void FourierModes::setBaseFunction(RtoR::Function_ptr func) { baseFunc = func; }

    void FourierModes::set_xMin(DevFloat val) { xMin = val; }

    void FourierModes::setL(DevFloat val) { L = val; }

    void FourierModes::setNSamples(CountType N) { samples = N; }


} // RtoR