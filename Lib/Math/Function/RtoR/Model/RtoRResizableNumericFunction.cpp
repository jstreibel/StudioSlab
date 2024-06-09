//
// Created by joao on 16/04/2021.
//

#include "RtoRResizableNumericFunction.h"

namespace Slab::Math {

    Real RtoR::ResizableNumericFunction::operator()(Real x) const {
        if (X.empty()) return .0;

        const Real ratio = x / (xMax - xMin);

        if (ratio >= 0 && ratio < 1) {
            unsigned i = ratio * (Real) X.size();
            return X[i];
        } else {
            return NaN;
        }

        //return (ratio >= 0 && ratio < 1) ? (unsigned)(ratio*(Real)X.size()) : NaN;
    }

    void RtoR::ResizableNumericFunction::insertBack(Real y) {
        if (y > yMax) yMax = y;
        if (y < yMin) yMin = y;

        X.push_back(y);
    }

    RtoR::ResizableNumericFunction::ResizableNumericFunction(Real xMin, Real xMax) : xMin(xMin), xMax(xMax) {}

    Real RtoR::ResizableNumericFunction::getYMin() const {
        return yMin;
    }

    Real RtoR::ResizableNumericFunction::getYMax() const {
        return yMax;
    }

}