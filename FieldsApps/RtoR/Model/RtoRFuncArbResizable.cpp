//
// Created by joao on 16/04/2021.
//

#include "RtoRFuncArbResizable.h"

Real RtoR::FuncArbResizable::operator()(Real x) const {
    if(X.empty()) return .0;

    const Real ratio = x/(xMax - xMin);

    if (ratio >= 0 && ratio < 1){
        unsigned i = ratio*(Real)X.size();
        return X[i];
    } else {
        return NaN;
    }

    //return (ratio >= 0 && ratio < 1) ? (unsigned)(ratio*(Real)X.size()) : NaN;
}

void RtoR::FuncArbResizable::insertBack(Real y) {
    if(y > yMax) yMax = y;
    if(y < yMin) yMin = y;

    X.push_back(y);
}

RtoR::FuncArbResizable::FuncArbResizable(Real xMin, Real xMax) : xMin(xMin), xMax(xMax) { }

Real RtoR::FuncArbResizable::getYMin() const {
    return yMin;
}

Real RtoR::FuncArbResizable::getYMax() const {
    return yMax;
}

