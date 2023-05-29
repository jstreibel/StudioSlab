//
// Created by joao on 15/10/2019.
//

#include "StraightLine.h"

RtoR2::StraightLine::StraightLine(Real2D x0, Real2D xf, Real sMin, Real sMax)
    : x0(x0), r(xf-x0), sMin(sMin), sMax(sMax) {   }

RtoR2::StraightLine::StraightLine(const RtoR2::StraightLine &line)
    : x0(line.x0), r(line.r), sMin(line.sMin), sMax(line.sMax) {   }

Real2D RtoR2::StraightLine::operator()(Real s) const {
    auto _s = (s-sMin)/(sMax-sMin);
    return x0 + r*_s;
}

RtoR2::StraightLine &RtoR2::StraightLine::operator=(const RtoR2::StraightLine &rhs) {
    x0 = rhs.x0;
    r = rhs.r;
    sMin = rhs.sMin;
    sMax = rhs.sMax;

    return *this;
}

RtoR2::StraightLine operator*(const Transform &T, const RtoR2::StraightLine &line) {
    Real2D x0 = line.getx0();
    Real2D xf = x0 + line.getr();

    return {T*x0, T*xf, line.getSMin(), line.getSMax()};
}


