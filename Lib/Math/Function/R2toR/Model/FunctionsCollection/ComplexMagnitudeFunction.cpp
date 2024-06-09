//
// Created by joao on 11/05/24.
//

#include "ComplexMagnitudeFunction.h"

namespace Slab::Math::R2toR {

    ComplexMagnitudeFunction::ComplexMagnitudeFunction() = default;

    ComplexMagnitudeFunction::ComplexMagnitudeFunction(std::shared_ptr<R2toC::NumericFunction> func)
    : function(func) {
    }

    Real ComplexMagnitudeFunction::operator()(Real2D x) const {
        if(function== nullptr) return NaN;

        auto val = (*function)(x);

        return norm(val);
    }

    void ComplexMagnitudeFunction::setFunc(std::shared_ptr<R2toC::NumericFunction> func) {
        function = func;
    }


} // R2toR