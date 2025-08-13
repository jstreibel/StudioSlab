//
// Created by joao on 24/06/2021.
//

#include "NoiseFunction.h"

#include <random>


namespace Slab::Math {

    RtoR::NoiseFunction::NoiseFunction(DevFloat min, DevFloat max) : min(min), max(max) {
        std::default_random_engine randomEngine(1);

        randomEngine();

        throw "Noise function not implemented.";
    }

    DevFloat RtoR::NoiseFunction::operator()(DevFloat x) const {
        return 0;
    }


}