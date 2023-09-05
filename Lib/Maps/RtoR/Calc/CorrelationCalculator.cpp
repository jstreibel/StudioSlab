//
// Created by joao on 21/08/23.
//

#include "CorrelationCalculator.h"

void CorrelationCalculator::setFunc(RtoR::Function::Ptr function) {
    func = std::move(function);
}

RtoR::Function::Ptr CorrelationCalculator::compute(Real begin, Real end, Real dx) {
    throw "Not implemented.";
    return func;
}
