//
// Created by joao on 24/06/2021.
//

#include "NoiseFunction.h"

#include <random>


RtoR::NoiseFunction::NoiseFunction(Real min, Real max) : min(min), max(max) {
    std::default_random_engine randomEngine(1);

    randomEngine();

    throw "Noise function not implemented.";
}

Real RtoR::NoiseFunction::operator()(Real x) const {
    return 0;
}

