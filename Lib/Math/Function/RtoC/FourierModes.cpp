//
// Created by joao on 11/09/23.
//

#include "FourierModes.h"

#include <utility>

namespace Slab::Math::RtoC {
    FourierModes::FourierModes(RtoR::Function::Ptr baseFunc, Real xMin, Real L, Count samples)
    : baseFunc(std::move(baseFunc)), xMin(xMin), L(L), samples(samples) {

    }

    Complex FourierModes::operator()(Real k) const {
        if(baseFunc == nullptr) return {};

        fix dx = L/(Real)samples;

        auto &f = *baseFunc;

        fix ik = Complex(0,k);
        auto aₖ = Complex();
        for(auto n=0; n<samples; ++n) {
           fix x = xMin + (Real)n*dx;

           aₖ += f(x)*std::exp(-ik*x);
        }

        return aₖ*dx;
    }

    void FourierModes::setBaseFunction(RtoR::Function::Ptr func) { baseFunc = func; }

    void FourierModes::set_xMin(Real val) { xMin = val; }

    void FourierModes::setL(Real val) { L = val; }

    void FourierModes::setNSamples(Count N) { samples = N; }


} // RtoR