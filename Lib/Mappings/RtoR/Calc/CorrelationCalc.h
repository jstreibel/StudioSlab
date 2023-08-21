//
// Created by joao on 21/08/23.
//

#ifndef STUDIOSLAB_CORRELATIONCALC_H
#define STUDIOSLAB_CORRELATIONCALC_H


#include <utility>

#include "Mappings/RtoR/Model/RtoRFunction.h"

class CorrelationCalc {
    RtoR::Function::Ptr func;

public:
    explicit CorrelationCalc(RtoR::Function::Ptr function=nullptr) : func(std::move(function)) {};

    void setFunc(RtoR::Function::Ptr function);

    RtoR::Function::Ptr compute(Real begin, Real end, Real dx);
};


#endif //STUDIOSLAB_CORRELATIONCALC_H
