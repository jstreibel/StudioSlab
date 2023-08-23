//
// Created by joao on 21/08/23.
//

#ifndef STUDIOSLAB_CORRELATIONCALCULATOR_H
#define STUDIOSLAB_CORRELATIONCALCULATOR_H


#include <utility>

#include "Mappings/RtoR/Model/RtoRFunction.h"

class CorrelationCalculator {
    RtoR::Function::Ptr func;

public:
    explicit CorrelationCalculator(RtoR::Function::Ptr function=nullptr) : func(std::move(function)) {};

    void setFunc(RtoR::Function::Ptr function);

    RtoR::Function::Ptr compute(Real begin, Real end, Real dx);
};


#endif //STUDIOSLAB_CORRELATIONCALCULATOR_H
