//
// Created by joao on 16/04/2021.
//

#include "RtoRResizableDiscreteFunction.h"

namespace Slab::Math {

    Real RtoR::ResizableDiscreteFunction::operator()(Real x) const {
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

    void RtoR::ResizableDiscreteFunction::insertBack(Real y) {
        if (y > yMax) yMax = y;
        if (y < yMin) yMin = y;

        X.push_back(y);
    }

    RtoR::ResizableDiscreteFunction::ResizableDiscreteFunction(Real xMin, Real xMax) : xMin(xMin), xMax(xMax) {}

    Real RtoR::ResizableDiscreteFunction::getYMin() const {
        return yMin;
    }

    Real RtoR::ResizableDiscreteFunction::getYMax() const {
        return yMax;
    }

}