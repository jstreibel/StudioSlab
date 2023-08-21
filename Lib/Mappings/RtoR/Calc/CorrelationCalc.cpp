//
// Created by joao on 21/08/23.
//

#include "CorrelationCalc.h"

void CorrelationCalc::setFunc(RtoR::Function::Ptr function) {
    func = std::move(function);
}

RtoR::Function::Ptr CorrelationCalc::compute(Real begin, Real end, Real dx) {
    return func;
}
