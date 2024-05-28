//
// Created by joao on 21/08/23.
//

#include "CorrelationCalculator.h"


namespace Slab::Math::RtoR {

    void CorrelationCalculator::setFunc(RtoR::Function_ptr function) {
        func = std::move(function);
    }

    RtoR::Function_ptr CorrelationCalculator::compute(Real begin, Real end, Real dx) {
        NOT_IMPLEMENTED_CLASS_METHOD
        return func;
    }

}