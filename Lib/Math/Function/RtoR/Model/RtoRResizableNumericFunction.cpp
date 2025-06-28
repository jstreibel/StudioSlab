//
// Created by joao on 16/04/2021.
//

#include "RtoRResizableNumericFunction.h"

namespace Slab::Math {

    DevFloat RtoR::ResizableNumericFunction::operator()(DevFloat x) const {
        if (X.empty()) return .0;

        const DevFloat ratio = x / (xMax - xMin);

        if (ratio >= 0 && ratio < 1) {
            unsigned i = ratio * (DevFloat) X.size();
            return X[i];
        } else {
            return NaN;
        }

        //return (ratio >= 0 && ratio < 1) ? (unsigned)(ratio*(Real)X.size()) : NaN;
    }

    void RtoR::ResizableNumericFunction::insertBack(DevFloat y) {
        if (y > yMax) yMax = y;
        if (y < yMin) yMin = y;

        X.push_back(y);
    }

    RtoR::ResizableNumericFunction::ResizableNumericFunction(DevFloat xMin, DevFloat xMax) : xMin(xMin), xMax(xMax) {}

    DevFloat RtoR::ResizableNumericFunction::getYMin() const {
        return yMin;
    }

    DevFloat RtoR::ResizableNumericFunction::getYMax() const {
        return yMax;
    }

}