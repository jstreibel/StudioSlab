//
// Created by joao on 21/08/23.
//

#ifndef STUDIOSLAB_CORRELATIONCALCULATOR_H
#define STUDIOSLAB_CORRELATIONCALCULATOR_H


#include <utility>

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class CorrelationCalculator {
        RtoR::Function_ptr func;

    public:
        explicit CorrelationCalculator(RtoR::Function_ptr function = nullptr) : func(std::move(function)) {};

        void setFunc(RtoR::Function_ptr function);

        RtoR::Function_ptr compute(Real begin, Real end, Real dx);
    };


}

#endif //STUDIOSLAB_CORRELATIONCALCULATOR_H
